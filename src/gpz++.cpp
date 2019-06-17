#include "gpz++.hpp"
#include <vif/core/main.hpp>

#ifndef GPZPP_GIT_HASH
#define GPZPP_GIT_HASH ""
#endif

const char* gpzpp_version = "1.0.0";
const char* gpzpp_git_hash = GPZPP_GIT_HASH;

int vif_main(int argc, char* argv[]) {
    std::string param_file = (argc >= 2 ? argv[1] : "gpz.param");

    // Setup
    options_t opts;
    PHZ_GPz::GPz gpz;
    if (!read_config(param_file, opts, gpz)) {
        return 1;
    }

    bool no_model = true;
    if (file::exists(opts.model_file)) {
        no_model = false;
    }

    if (!opts.reuse_model || no_model) {
        // Train

        // Read existing model if asked
        PHZ_GPz::GPzModel model;
        if (!no_model && opts.use_model_as_hint) {
            if (!read_model(opts, model)) {
                return 1;
            }
        }

        // Read data
        PHZ_GPz::Vec2d input, input_error;
        PHZ_GPz::Vec1d output, output_weight;
        if (!read_training(opts, input, input_error, output, output_weight)) {
            return 1;
        }

        // Do training
        try {
            gpz.fit(input, input_error, output, output_weight, model);
        } catch (std::exception& e) {
            error("an exception occured during the training");
            error(e.what());
            return 1;
        }

        if (opts.save_model) {
            // Write model
            write_model(opts, gpz.getModel());
        }
    } else {
        // Load existing model
        PHZ_GPz::GPzModel model;
        if (!read_model(opts, model)) {
            return 1;
        }

        try {
            gpz.loadModel(model);
        } catch (std::exception& e) {
            error("an exception occured while loading the model");
            error(e.what());
            return 1;
        }
    }

    if (!opts.prediction_catalog.empty()) {
        // Predict

        // Read data
        PHZ_GPz::Vec2d input, input_error;
        vec1s id;
        if (!read_prediction(opts, id, input, input_error)) {
            return 1;
        }

        // Do prediction
        PHZ_GPz::GPzOutput out;
        try {
            out = gpz.predict(input, input_error);
        } catch (std::exception& e) {
            error("an exception occured while making predictions");
            error(e.what());
            return 1;
        }

        // Write output to disk
        write_output(opts, gpz, id, out);
    }

    return 0;
}
