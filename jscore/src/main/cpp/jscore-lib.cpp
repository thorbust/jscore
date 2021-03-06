//
// Created by George on 15/01/2019.
//

#include <jni.h>
#include <string>

#include "libplatform/libplatform.h"
#include "v8.h"
#include "pre.h"

#include <fstream>
#include <iostream>

#include <android/log.h>
#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>


std::unique_ptr<v8::Platform> platform;
v8::Isolate *isolate;
v8::Persistent<v8::Context> persistentContext;

// Begins preprocessing actions and then initializes v8 and loads
// the virtual machine into memory.
extern "C" JNIEXPORT void JNICALL
Java_jscore_android_JSContext_v8(
        JNIEnv *env,
        jobject /* this */) {

    // Begin actions needed before initializing v8
    auto * pre = new js::core::Pre();
    pre->Initialize();

    // Initialize V8.
    v8::V8::InitializeICU();
    platform = v8::platform::NewDefaultPlatform();
    v8::V8::InitializePlatform(&(*platform));
    v8::V8::Initialize();

    // Create a new Isolate and make it the current one.
    v8::Isolate::CreateParams create_params;
    create_params.array_buffer_allocator = v8::ArrayBuffer::Allocator::NewDefaultAllocator();
    isolate = v8::Isolate::New(create_params);

    v8::Isolate::Scope isolate_scope(isolate);
    // Create a stack-allocated handle scope.
    v8::HandleScope handle_scope(isolate);

    // Create a new context.
    v8::Local<v8::Context> context = v8::Context::New(isolate);

    // attach the context to the persistent context, to avoid V8 GC-ing it
    persistentContext.Reset(isolate, context);
}

extern "C" JNIEXPORT jstring
JNICALL
Java_jscore_android_JSContext_evaluate(
        JNIEnv *env,
        jobject /* this */,
        jstring string) {


    std::string test_script = env->GetStringUTFChars(string, nullptr);

    std::string test_result;

    std::string hello = "Hello v8 from C++!\n";

    v8::Isolate::Scope isolate_scope(isolate);
    v8::HandleScope handle_scope(isolate);

    // Enter the context for compiling and running the hello world script.
    v8::Local<v8::Context> context = v8::Local<v8::Context>::New(isolate, persistentContext);
    v8::Context::Scope context_scope(context);

    // Create a string containing the JavaScript source code.
    v8::Local<v8::String> source = v8::String::NewFromUtf8(
            isolate, test_script.c_str(), v8::NewStringType::kNormal).ToLocalChecked();

    // Compile the source code.
    v8::Local<v8::Script> script =
            v8::Script::Compile(context, source).ToLocalChecked();

    // Run the script to get the result.
    v8::Local<v8::Value> result = script->Run(context).ToLocalChecked();

    // Convert the result to an UTF8 string and print it.
    v8::String::Utf8Value utf8(isolate, result);
    //printf("%s\n", *utf8);
    test_result += *utf8;

    return env->NewStringUTF(test_result.c_str());

}

extern "C" JNIEXPORT void JNICALL
Java_jscore_android_JSContext_dispose(
        JNIEnv *env,
        jobject) {
    isolate->Dispose();
    v8::V8::Dispose();
    v8::V8::ShutdownPlatform();
}

//extern "C" JNIEXPORT jstring JNICALL
//Java_jscore_android_JSContext_evaluate(
//        JNIEnv *env,
//        jobject /* this */,
//        jstring string) {
//
//    __android_log_print(ANDROID_LOG_INFO, "Test Asset Manager", "The value is");
//
//    std::string test_script = env->GetStringUTFChars(string, nullptr);
//
//    std::string hello;
//
////    AAssetManager *mgr = AAssetManager_fromJava(env, assetManager);
////    AAssetDir* assetDir = AAssetManager_openDir(mgr, "snapshots");
////    const char* filename = (const char*)NULL;
////    while ((filename = AAssetDir_getNextFileName(assetDir)) != NULL) {
////        AAsset* asset = AAssetManager_open(mgr, filename, AASSET_MODE_STREAMING);
////        char buf[BUFSIZ];
////        int nb_read = 0;
////        FILE* out = fopen(filename, "w");
////        while ((nb_read = AAsset_read(asset, buf, BUFSIZ)) > 0)
////            fwrite(buf, nb_read, 1, out);
////        fclose(out);
////        AAsset_close(asset);
////    }
////    AAssetDir_close(assetDir);
//
////    std::string line;
////    std::ifstream myfile ("../test.txt");
////    if (myfile.is_open())
////    {
////        v8::V8::InitializeExternalStartupData("natives_blob.bin", "snapshot_blob.bin");
////
////        while ( getline (myfile,line) )
////        {
////            std::cout << line << '\n';
////        }
////        myfile.close();
////    }
//
////    typedef void (*setter_fn)(v8::StartupData *);
//
//
//
//
////    adb shell 'mkdir -p /data/local/tmp/v8/bin'
////
////    JSCore/jscore/src/main/cpp on  master [!?]
////    ➜ adb push natives_blob.bin data/local/tmp/v8/bin
////    natives_blob.bin: 1 file pushed. 3.2 MB/s (11764 bytes in 0.004s)
////
////    JSCore/jscore/src/main/cpp on  master [!?]
////    ➜ adb push snapshot_blob.bin data/local/tmp/v8/bin
////    snapshot_blob.bin: 1 file pushed. 28.3 MB/s (235148 bytes in 0.008s)
//    //v8::V8::InitializeExternalStartupData("/data/local/tmp/v8/bin/natives_blob.bin", "/data/local/tmp/v8/bin/snapshot_blob.bin");
//
//    v8::Persistent<v8::Context> persistentContext;
//
//
//    std::unique_ptr<v8::Platform> platform = v8::platform::NewDefaultPlatform();
//    v8::V8::InitializePlatform(platform.get());
//    v8::V8::Initialize();
//
//    v8::Isolate::CreateParams create_params;
//    create_params.array_buffer_allocator =
//            v8::ArrayBuffer::Allocator::NewDefaultAllocator();
//
//    v8::Isolate* isolate = v8::Isolate::New(create_params);
//    {
//        v8::Isolate::Scope isolate_scope(isolate);
//        // Create a stack-allocated handle scope.
//        v8::HandleScope handle_scope(isolate);
//        // Create a new context.
//        v8::Local<v8::Context> context = v8::Context::New(isolate);
//
//        persistentContext.Reset(isolate, context);
//        // Enter the context for compiling and running the hello world script.
//        v8::Context::Scope context_scope(context);
//        {
//            // Create a string containing the JavaScript source code.
//            v8::Local<v8::String> source =
//                    v8::String::NewFromUtf8(isolate, test_script.c_str(),
//                                            v8::NewStringType::kNormal)
//                            .ToLocalChecked();
//            // Compile the source code.
//            v8::Local<v8::Script> script =
//                    v8::Script::Compile(context, source).ToLocalChecked();
//            // Run the script to get the result.
//            v8::Local<v8::Value> result = script->Run(context).ToLocalChecked();
//            // Convert the result to an UTF8 string and print it.
//            v8::String::Utf8Value utf8(isolate, result);
//            printf("%s\n", *utf8);
//
//            hello += *utf8;
//        }
//    }
//
////    isolate->Dispose();
////    v8::V8::Dispose();
////    v8::V8::ShutdownPlatform();
////    delete create_params.array_buffer_allocator;
//
//    return env->NewStringUTF(hello.c_str());
//}
