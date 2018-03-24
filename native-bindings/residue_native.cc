#include <nan.h>
#include <residue/residue.h>

using v8::FunctionTemplate;
using v8::Handle;
using v8::Object;
using v8::Isolate;
using v8::String;
using Nan::To;
using Nan::GetFunction;
using Nan::New;
using Nan::Set;

NAN_METHOD(Version)
{
    info.GetReturnValue().Set(String::NewFromUtf8(Isolate::GetCurrent(), Residue::version().c_str()));
}

NAN_METHOD(Configure)
{
    String::Utf8Value jsonParam(info[0]->ToString());
    std::string json(*jsonParam);
    Residue::loadConfigurationFromJson(json);
}

NAN_METHOD(LoadConnection)
{
    String::Utf8Value jsonParam(info[0]->ToString());
    std::string json(*jsonParam);
    Residue::loadConnectionFromJson(json);
}

NAN_METHOD(Connect)
{
    Residue::reconnect();
}

#define DEFINE_LOG_FN(level)                             \
String::Utf8Value paramLoggerId(info[0]->ToString());    \
String::Utf8Value paramFile(info[1]->ToString());        \
String::Utf8Value paramLine(info[2]->ToString());        \
String::Utf8Value paramFunc(info[3]->ToString());        \
String::Utf8Value paramMsg(info[4]->ToString());         \
std::string loggerId(*paramLoggerId);                    \
std::string file(*paramFile);                            \
std::string line(*paramLine);                            \
std::string func(*paramFunc);                            \
std::string msg(*paramMsg);                              \
el::base::type::LineNumber lineNumb = 0;                 \
if (line != "undefined" && line != "null") {             \
    lineNumb = stoi(line);                               \
}                                                        \
el::base::Writer(level, file.c_str(), lineNumb, func.c_str(), el::base::DispatchAction::NormalLog).\
    construct(1, loggerId.c_str()) << msg

NAN_METHOD(Info)
{
    DEFINE_LOG_FN(el::Level::Info);
}

NAN_METHOD(Debug)
{
    DEFINE_LOG_FN(el::Level::Debug);
}

#undef DEFINE_LOG_FN

NAN_MODULE_INIT(InitAll)
{
    #define DEFINE_FN(name, callback) Set(target, New<String>(#name).ToLocalChecked(), \
        GetFunction(New<FunctionTemplate>(callback)).ToLocalChecked())

    DEFINE_FN(version, Version);
    DEFINE_FN(configure, Configure);
    DEFINE_FN(load_connection, LoadConnection);
    DEFINE_FN(connect, Connect);
    DEFINE_FN(info, Info);
    DEFINE_FN(debug, Debug);

    #undef DEFINE_FN
}


NODE_MODULE(residue_native, InitAll)
