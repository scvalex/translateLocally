#include "MarianInterface.h"
#include "3rd_party/bergamot-translator/src/translator/service.h"
#include "3rd_party/bergamot-translator/src/translator/service.h"
#include "3rd_party/bergamot-translator/src/translator/parser.h"
#include "3rd_party/bergamot-translator/src/translator/response.h"
#include "3rd_party/bergamot-translator/3rd_party/marian-dev/src/3rd_party/spdlog/spdlog.h"
#include <thread>

namespace  {
marian::Ptr<marian::Options> MakeOptions(QString path_to_model_dir) {
    std::string model_path = path_to_model_dir.toStdString() + "config.intgemm8bitalpha.yml";
    std::vector<std::string> args = {"marian-decoder", "-c", model_path, "--cpu-threads", "8", "--mini-batch-words", "1000"};

    std::vector<char *> argv;
    argv.reserve(args.size());

    for (int i = 0; i < args.size(); ++i) {
        argv.push_back(const_cast<char *>(args[i].c_str()));
    }
    auto cp = marian::bergamot::createConfigParser();
    auto options = cp.parseOptions(argv.size(), &argv[0], true);
    return options;
}
} // Anonymous namespace

MarianInterface::MarianInterface(QString path_to_model_dir, QObject *parent)
    : QObject(parent)
    , service_(new marian::bergamot::Service(MakeOptions(path_to_model_dir))) {}

void MarianInterface::translate(QString in) {
    // Wait on future until Response is complete. Since the future doesn't have a callback or anything
    // we should put all the processing in a background thread. Normally, if we have a future, we expect
    // that future to have a method that allows to attach a callback, but this is reserved for c++20? c++22
    // We have to copy any member variables we use (I'm looking at you QString input, because QString is copy-on-write)
    auto translateAndSignal = [&, in]{
        std::string input = in.toStdString();
        using marian::bergamot::Response;
        std::future<marian::bergamot::Response> responseFuture = service_->translate(std::move(input));
        responseFuture.wait();
        marian::bergamot::Response response = responseFuture.get();
        emit translationReady(QString::fromStdString(response.target.text));
    };
    std::thread mythread(translateAndSignal);
    mythread.detach();
}

MarianInterface::~MarianInterface() {
    // We need to manually destroy the loggers, as marian doesn't do that.
    spdlog::drop("general");
    spdlog::drop("valid");
}

