#include "GPz++.hpp"

std::string remove_first_last(std::string val, std::string charlist) {
    if (val.empty()) return val;
    uint_t p0 = 0, n = val.size();
    if (charlist.find_first_of(val[0]) != charlist.npos) {
        ++p0; --n;
    }
    if (n > 0 && charlist.find_first_of(val[val.size()-1]) != charlist.npos) {
        --n;
    }

    return val.substr(p0, n);
}

template <typename T>
bool parse_value_impl(const std::string& val, T& out) {
    return from_string(val, out);
}

bool parse_value_impl(const std::string& val, std::string& out) {
    out = remove_first_last(val, "\'\"");
    return true;
}

bool parse_value_impl(const std::string& val, PHZ_GPz::CovarianceType& out) {
    if (val == "gpgl") {
        out = PHZ_GPz::CovarianceType::GLOBAL_LENGTH;
    } else if (val == "gpvl") {
        out = PHZ_GPz::CovarianceType::VARIABLE_LENGTH;
    } else if (val == "gpgd") {
        out = PHZ_GPz::CovarianceType::GLOBAL_DIAGONAL;
    } else if (val == "gpvd") {
        out = PHZ_GPz::CovarianceType::VARIABLE_DIAGONAL;
    } else if (val == "gpgc") {
        out = PHZ_GPz::CovarianceType::GLOBAL_COVARIANCE;
    } else if (val == "gpvc") {
        out = PHZ_GPz::CovarianceType::VARIABLE_COVARIANCE;
    } else {
        return false;
    }
    return true;
}

bool parse_value_impl(const std::string& val, PHZ_GPz::PriorMeanFunction& out) {
    if (val == "none") {
        out = PHZ_GPz::PriorMeanFunction::ZERO;
    } else if (val == "constant") {
        out = PHZ_GPz::PriorMeanFunction::CONSTANT_PREPROCESS;
    } else if (val == "linear") {
        out = PHZ_GPz::PriorMeanFunction::LINEAR_PREPROCESS;
    } else if (val == "linear_marg") {
        out = PHZ_GPz::PriorMeanFunction::LINEAR_MARGINALIZE;
    } else {
        return false;
    }
    return true;
}

bool parse_value_impl(const std::string& val, PHZ_GPz::WeightingScheme& out) {
    if (val == "uniform") {
        out = PHZ_GPz::WeightingScheme::UNIFORM;
    } else if (val == "1/(1+z)") {
        out = PHZ_GPz::WeightingScheme::ONE_OVER_ONE_PLUS_OUTPUT;
    } else if (val == "balanced") {
        out = PHZ_GPz::WeightingScheme::BALANCED;
    } else {
        return false;
    }
    return true;
}

bool parse_value_impl(const std::string& val, PHZ_GPz::NormalizationScheme& out) {
    if (val == "natural") {
        out = PHZ_GPz::NormalizationScheme::NATURAL;
    } else if (val == "whiten") {
        out = PHZ_GPz::NormalizationScheme::WHITEN;
    } else {
        return false;
    }
    return true;
}

bool parse_value_impl(const std::string& val, PHZ_GPz::TrainValidationSplitMethod& out) {
    if (val == "random") {
        out = PHZ_GPz::TrainValidationSplitMethod::RANDOM;
    } else if (val == "sequential") {
        out = PHZ_GPz::TrainValidationSplitMethod::SEQUENTIAL;
    } else {
        return false;
    }
    return true;
}

bool parse_value_impl(const std::string& val, PHZ_GPz::OutputUncertaintyType& out) {
    if (val == "uniform") {
        out = PHZ_GPz::OutputUncertaintyType::UNIFORM;
    } else if (val == "input_dependent") {
        out = PHZ_GPz::OutputUncertaintyType::INPUT_DEPENDENT;
    } else {
        return false;
    }
    return true;
}

template <typename T>
bool parse_value_impl(std::string val, vec<1,T>& out) {
    if (val.empty()) return true;
    val = remove_first_last(val, "[]");
    vec1s spl = trim(split(val, ","));

    if (spl.size() == 1 && spl[0].empty()) {
        out.clear();
        return true;
    }

    out.resize(spl.size());
    for (uint_t i : range(spl)) {
        if (!parse_value_impl(spl[i], out[i])) {
            return false;
        }
    }

    return true;
}

template <typename T>
bool parse_value(const std::string& key, const std::string& val, T& out) {
    if (!parse_value_impl(val, out)) {
        error("could not parse value of parameter ", key);
        note("could not convert '", val, "' into ", pretty_type_t(T));
        return false;
    }

    return true;
}

bool read_config(const std::string& filename, options_t& opts, PHZ_GPz::GPz& gpz) {
    std::ifstream in(filename);
    if (!in) {
        error("could not open param file '", filename, "'");
        return false;
    }

    vec1s unparsed_key, unparsed_val;

    PHZ_GPz::GPzOptimizations optim;

    auto do_parse = [&](const std::string& key, const std::string& val) {
        #define PARSE_OPTION(name) if (key == #name) { return parse_value(key, val, opts.name); }
        #define PARSE_OPTION_RENAME(opt, name) if (key == name) { return parse_value(key, val, opts.opt); }
        #define PARSE_OPTION_GPZ(name, type, func) if (key == #name) { type tmp; if (parse_value(key, val, tmp)) { gpz.func(tmp); return true; } else { return false; } }
        #define PARSE_OPTION_GPZ_OPTIM(name, field) if (key == #name) { return parse_value(key, val, optim.field); }

        PARSE_OPTION(training_catalog)
        PARSE_OPTION(prediction_catalog)
        PARSE_OPTION(output_catalog)
        PARSE_OPTION(model_file)
        PARSE_OPTION(save_model)
        PARSE_OPTION(reuse_model)
        PARSE_OPTION(use_model_as_hint)
        PARSE_OPTION(output_column)
        PARSE_OPTION(weight_column)
        PARSE_OPTION(flux_column_prefix)
        PARSE_OPTION(error_column_prefix)
        PARSE_OPTION(use_errors)
        PARSE_OPTION(output_min)
        PARSE_OPTION(output_max)
        PARSE_OPTION(transform_inputs)
        PARSE_OPTION_RENAME(bands_regex, "bands")

        PARSE_OPTION_GPZ(verbose,                       bool,                                setVerboseMode)
        PARSE_OPTION_GPZ(num_bf,                        uint_t,                              setNumberOfBasisFunctions)
        PARSE_OPTION_GPZ(covariance,                    PHZ_GPz::CovarianceType,             setCovarianceType)
        PARSE_OPTION_GPZ(prior_mean,                    PHZ_GPz::PriorMeanFunction,          setPriorMeanFunction)
        PARSE_OPTION_GPZ(weighting_scheme,              PHZ_GPz::WeightingScheme,            setWeightingScheme)
        PARSE_OPTION_GPZ(normalization_scheme,          PHZ_GPz::NormalizationScheme,        setNormalizationScheme)
        PARSE_OPTION_GPZ(valid_sample_method,           PHZ_GPz::TrainValidationSplitMethod, setTrainValidationSplitMethod)
        PARSE_OPTION_GPZ(output_error_type,             PHZ_GPz::OutputUncertaintyType,      setOutputUncertaintyType)
        PARSE_OPTION_GPZ(balanced_weighting_bin,        double,                              setBalancedWeightingBinSize)
        PARSE_OPTION_GPZ(balanced_weighting_max_weight, double,                              setBalancedWeightingMaxWeight)
        PARSE_OPTION_GPZ(train_valid_ratio,             double,                              setTrainValidationRatio)
        PARSE_OPTION_GPZ(valid_sample_seed,             uint_t,                              setTrainValidationSplitSeed)
        PARSE_OPTION_GPZ(bf_position_seed,              uint_t,                              setInitialPositionSeed)
        PARSE_OPTION_GPZ(fuzzing,                       bool,                                setFuzzInitialValues)
        PARSE_OPTION_GPZ(fuzzing_seed,                  uint_t,                              setFuzzingSeed)
        PARSE_OPTION_GPZ(max_iter,                      uint_t,                              setOptimizationMaxIterations)
        PARSE_OPTION_GPZ(tolerance,                     double,                              setOptimizationTolerance)
        PARSE_OPTION_GPZ(grad_tolerance,                double,                              setOptimizationGradientTolerance)
        PARSE_OPTION_GPZ(predict_error,                 bool,                                setPredictVariance)

        PARSE_OPTION_GPZ_OPTIM(n_thread, maxThreads)

        #undef  PARSE_OPTION
        #undef  PARSE_OPTION_RENAME
        #undef  PARSE_OPTION_GPZ
        #undef  PARSE_OPTION_GPZ_OPTIM

        unparsed_key.push_back(key);
        unparsed_val.push_back(val);

        return true;
    };

    uint_t l = 0;
    std::string line;
    while (ascii::getline(in, line)) {
        ++l;

        line = trim(line);
        if (line.empty() || line[0] == '#') continue;

        // Remove inline comments
        auto cp = line.find_first_of('#');
        if (cp != line.npos) {
            line = line.substr(0, cp);
        }

        // Split on '=' and parse key and values
        auto eqp = line.find_first_of('=');
        if (eqp == line.npos) {
            error("ill formed line in configuration file");
            note(line);
            return false;
        }

        std::string key = to_lower(trim(line.substr(0, eqp)));
        std::string val = trim(line.substr(eqp+1));

        if (!do_parse(key, val)) {
            return false;
        }
    }

    for (uint_t p : range(unparsed_key)) {
        warning("unknown parameter '", to_upper(unparsed_key[p]), "'");
    }

    // Check and adjust options

    if (optim.maxThreads > 1) {
        optim.enableMultithreading = true;
    }

    if (opts.output_catalog.empty()) {
        opts.output_catalog = "gpz.cat";
    }

    if (opts.model_file.empty()) {
        opts.model_file = "gpz_model.dat";
    }

    if (optim.maxThreads > 100) {
        error("asking for more than 100 threads (", optim.maxThreads, ") is asking for trouble!");
        error("please double check the value of N_THREAD=...");
        return false;
    }

    if (opts.training_catalog.empty() && !(opts.reuse_model && file::exists(opts.model_file))) {
        error("GPz++ needs either a training catalog or a trained model before it can do predictions");
        error("please specify either TRAINING_CATALOG=...");
        error("... or set REUSE_MODEL=1 and provide a valid MODEL_FILE=...");
        return false;
    }

    if (opts.training_catalog.empty() && opts.prediction_catalog.empty()) {
        error("no training or prediction catalog provided, nothing to do");
        error("please specify either TRAINING_CATALOG=... or PREDICTION_CATALOG=...");
        return false;
    }

    if (opts.flux_column_prefix.empty() && opts.error_column_prefix.empty() && opts.use_errors) {
        error("impossible to identify error columns if FLUX_COLUMN_PREFIX = ERROR_COLUMN_PREFIX"
            " or if both are empty.");
        error("Please specify values for these parameters or set USE_ERRORS=0");
        return false;
    }
    if (opts.output_catalog == opts.training_catalog) {
        error("the chosen output catalog file name (", opts.output_catalog, ") would overwrite the "
            "training catalog");
        return false;
    }

    if (opts.output_catalog == opts.prediction_catalog) {
        error("the chosen output catalog file name (", opts.output_catalog, ") would overwrite the "
            "prediction input catalog");
        return false;
    }

    if (opts.output_catalog == opts.model_file && opts.save_model) {
        error("the chosen output catalog file name (", opts.output_catalog, ") would overwrite the "
            "output model");
        return false;
    }

    if (opts.reuse_model && opts.use_model_as_hint) {
        error("cannot set both REUSE_MODEL=1 and USE_MODEL_AS_HINT=1");
        return false;
    }

    vec1s allowed_transforms = {"none", "no", "", "flux_to_luptitude"};
    if (!is_any_of(opts.transform_inputs, allowed_transforms)) {
        error("unknown flux transformation '", opts.transform_inputs, "'");
        return false;
    }

    // Set optimization parameters
    gpz.setOptimizationFlags(optim);

    return true;
}

template<typename T>
bool read_vec1d(const std::string& line, T& v) {
    std::istringstream in(line);
    uint_t nelem = v.size();
    for (uint_t i : range(nelem)) {
        if (!(in >> v[i])) {
            return false;
        }
    }

    return true;
}

bool read_model(options_t& opts, PHZ_GPz::GPzModel& model) {
    std::string filename = opts.model_file;
    std::ifstream in(filename);
    if (!in) {
        error("could not open model file '", filename, "'");
        return false;
    }

    uint_t nfeature = 0;
    uint_t nbasis = 0;

    bool done = false;
    uint_t step = 0;
    uint_t l = 0;
    std::string line;
    while (!done && ascii::getline(in, line)) {
        ++l;

        line = trim(line);
        if (line.empty() || line[0] == '#') continue;

        if (step == 0) {
            if (!from_string(line, nfeature)) {
                error("could not read number of features from '", line, "'");
                error("reading ", filename, " on line ", l);
                return false;
            }

            model.featureMean.resize(nfeature);
            model.featureSigma.resize(nfeature);
            opts.bands.resize(nfeature);
        } else if (step == 1) {
            if (!read_vec1d(line, opts.bands)) {
                error("could not read feature column names");
                error("reading ", filename, " on line ", l);
                return false;
            }
        } else if (step == 2) {
            if (!read_vec1d(line, model.featureMean)) {
                error("could not read feature means");
                error("reading ", filename, " on line ", l);
                return false;
            }
        } else if (step == 3) {
            if (!read_vec1d(line, model.featureSigma)) {
                error("could not read feature sigmas");
                error("reading ", filename, " on line ", l);
                return false;
            }
        } else if (step == 4) {
            if (!from_string(line, model.outputMean)) {
                error("could not read output mean from '", line, "'");
                error("reading ", filename, " on line ", l);
                return false;
            }
        } else if (step == 5) {
            if (!from_string(line, nbasis)) {
                error("could not read number of basis functions from '", line, "'");
                error("reading ", filename, " on line ", l);
                return false;
            }

            model.modelWeights.resize(nbasis);
            model.modelInputPrior.resize(nbasis);
            model.modelInvCovariance.resize(nbasis,nbasis);
            model.parameters.basisFunctionLogRelevances.resize(nbasis);
            model.parameters.uncertaintyBasisWeights.resize(nbasis);
            model.parameters.uncertaintyBasisLogRelevances.resize(nbasis);
            model.parameters.basisFunctionPositions.resize(nbasis,nfeature);
            model.parameters.basisFunctionCovariances.resize(nbasis);
            for (uint_t i : range(nbasis)) {
                model.parameters.basisFunctionCovariances[i].resize(nfeature,nfeature);
            }
        } else if (step == 6) {
            if (!read_vec1d(line, model.modelWeights)) {
                error("could not read BF weights");
                error("reading ", filename, " on line ", l);
                return false;
            }
        } else if (step == 7) {
            if (!read_vec1d(line, model.modelInputPrior)) {
                error("could not read BF priors");
                error("reading ", filename, " on line ", l);
                return false;
            }
        } else if (step == 8) {
            if (!read_vec1d(line, model.parameters.basisFunctionLogRelevances)) {
                error("could not read BF log relevances");
                error("reading ", filename, " on line ", l);
                return false;
            }
        } else if (step == 9) {
            if (!read_vec1d(line, model.parameters.uncertaintyBasisWeights)) {
                error("could not read BF weights");
                error("reading ", filename, " on line ", l);
                return false;
            }
        } else if (step == 10) {
            if (!read_vec1d(line, model.parameters.uncertaintyBasisLogRelevances)) {
                error("could not read BF weights");
                error("reading ", filename, " on line ", l);
                return false;
            }
        } else if (step == 11) {
            if (!from_string(line, model.parameters.logUncertaintyConstant)) {
                error("could not read log uncertainty constant from '" , line, "'");
                error("reading ", filename, " on line ", l);
                return false;
            }
        } else if (step == 12) {
            for (uint_t i : range(nbasis)) {
                if (i != 0) {
                    if (!ascii::getline(in, line)) {
                        error("unexpected end of file");
                        error("reading ", filename, " on line ", l);
                        return false;
                    }
                    ++l;
                }

                PHZ_GPz::Vec1d tmp(nbasis);
                if (!read_vec1d(line, tmp)) {
                    error("could not read BF inverse covariance");
                    error("reading ", filename, " on line ", l);
                    return false;
                }

                model.modelInvCovariance.row(i) = tmp;
            }
        } else if (step == 13) {
            for (uint_t i : range(nbasis)) {
                if (i != 0) {
                    if (!ascii::getline(in, line)) {
                        error("unexpected end of file");
                        error("reading ", filename, " on line ", l);
                        return false;
                    }
                    ++l;
                }

                PHZ_GPz::Vec1d tmp(nfeature);
                if (!read_vec1d(line, tmp)) {
                    error("could not read BF ", i, " position");
                    error("reading ", filename, " on line ", l);
                    return false;
                }

                model.parameters.basisFunctionPositions.row(i) = tmp;
            }
        } else if (step == 14) {
            for (uint_t i : range(nbasis))
            for (uint_t j : range(nfeature)) {
                if (i != 0 || j != 0) {
                    if (!ascii::getline(in, line)) {
                        error("unexpected end of file");
                        error("reading ", filename, " on line ", l);
                        return false;
                    }
                    ++l;
                }

                PHZ_GPz::Vec1d tmp(nfeature);
                if (!read_vec1d(line, tmp)) {
                    error("could not read BF ", i, " covariance");
                    error("reading ", filename, " on line ", l);
                    return false;
                }

                model.parameters.basisFunctionCovariances[i].row(j) = tmp;
            }

            done = true;
        }

        ++step;
    }

    return true;
}

bool read_header(const std::string& filename, vec1s& header) {
    std::ifstream in(filename);
    std::string line;
    while (ascii::getline(in, line)) {
        line = trim(line);

        // Find the first non-empty comment line
        if (line.empty() || line[0] != '#') continue;

        line = trim(line.substr(1));
        if (line.empty()) continue;

        // Split column names by spaces
        header = to_lower(split_any_of(line, " \t\n\r"));
        return true;
    }

    error("missing header in '", filename, "'");
    note("the header line must start with # and list the column names");

    return false;
}

bool read_ascii(options_t& opts, const std::string& filename, vec1s& id, PHZ_GPz::Vec2d& input,
    PHZ_GPz::Vec2d& inputError, PHZ_GPz::Vec1d& output, PHZ_GPz::Vec1d& weight,
    const std::string& which) {

    if (!file::exists(filename)) {
        error("could not open ", which, " catalog '", filename, "'");
        return false;
    }

    // Read all lines to determine the number of elements
    uint_t ngal = 0; {
        std::ifstream in(filename);
        std::string line;
        while (ascii::getline(in, line)) {
            line = trim(line);
            if (line.empty() || line[0] == '#') continue;

            ++ngal;
        }
    }

    // Read header to determine number of features and other content
    vec1s header;
    if (!read_header(filename, header)) {
        return false;
    }

    uint_t col_id = npos;
    uint_t col_output = npos;
    uint_t col_weight = npos;
    vec1u col_flux, col_eflux;

    vec1b column_used(header.size());
    if (which == "training") {
        col_output = where_first(header == to_lower(opts.output_column));
        if (col_output == npos) {
            error("could not find output column '", opts.output_column, "'");
            error("in file '", filename, "'");
            return false;
        }

        column_used[col_output] = true;

        if (!opts.weight_column.empty()) {
            col_weight = where_first(header == to_lower(opts.weight_column));
            if (col_output == npos) {
                error("could not find weight column '", opts.weight_column, "'");
                error("in file '", filename, "'");
                return false;
            }

            column_used[col_weight] = true;
        }
    } else {
        col_id = where_first(header == "id");
        if (col_id != npos) {
            column_used[col_id] = true;
        }
    }

    vec1s bands;

    for (uint_t i : range(opts.bands_regex)) {
        vec1u fid = where(begins_with(header, to_lower(opts.flux_column_prefix)) &&
            regex_match(header, opts.bands_regex[i]) && !column_used);

        if (fid.empty()) {
            warning("no column found matching the regular expression '", opts.bands_regex[i], "'");
            continue;
        }

        if (opts.use_errors) {
            for (uint_t k : range(fid)) {
                std::string err_str = replace(header[fid[k]], opts.flux_column_prefix, opts.error_column_prefix);
                uint_t err_col = where_first(header == err_str);
                if (err_col == npos) {
                    warning("flux column ", header[fid[k]], " has no corresponding error column "
                        "and will be ignored");
                } else {
                    col_flux.push_back(fid[k]);
                    col_eflux.push_back(err_col);
                    column_used[fid[k]] = true;
                    bands.push_back(erase_begin(header[fid[k]], to_lower(opts.flux_column_prefix)));
                }
            }
        } else {
            append(col_flux, fid);
            append(bands, erase_begin(header[fid], to_lower(opts.flux_column_prefix)));
            column_used[fid] = true;
        }
    }

    if (bands.empty()) {
        error("no columns matching the feature selection (BANDS=", collapse(opts.bands_regex), ")");
        error("available columns: ", collapse(header, ","));
        return false;
    }

    inplace_sort(bands);

    if (opts.bands.empty()) {
        opts.bands = bands;
    } else {
        if (opts.bands.size() != bands.size() || count(opts.bands != bands) > 0) {
            std::string c1, c2;
            if (which == "training") {
                c1 = "stored model";
                c2 = "training catalog";
            } else {
                c1 = "training catalog";
                c2 = "prediction catalog";
            }

            error("mismatch of bands between ", c1, ":");
            error("  ", opts.bands);
            error("... and ", c2, ":");
            error("  ", bands);
            return false;
        }
    }

    uint_t nfeature = bands.size();

    // Resize arrays
    input.resize(ngal, nfeature);
    if (opts.use_errors) {
        inputError.resize(ngal, nfeature);
    }

    if (which == "training") {
        output.resize(ngal);
        if (col_weight != npos) {
            weight.resize(ngal);
        }
    } else {
        if (col_id != npos) {
            id.resize(ngal);
        }
    }

    // Read in data
    uint_t gid = 0;
    uint_t l = 0;
    std::ifstream in(filename);

    std::string line;
    while (ascii::getline(in, line)) {
        ++l;
        line = trim(line);
        if (line.empty() || line[0] == '#') continue;

        vec1s spl = split_any_of(line, " \t\n\r");

        if (spl.size() != header.size()) {
            error("line ", l, " has ", spl.size(), " columns while header has ", header.size());
            return false;
        }

        // Read ID
        if (col_id != npos) {
            id[gid] = spl[col_id];
        }

        // Read inputs
        vec1f flx;
        vec1b good = from_string(spl[col_flux], flx);
        if (count(!good) != 0) {
            for (uint_t b : where(!good)) {
                error("could not read feature (", header[col_flux[b]], ") from line ", l);
                note("must be a floating point number, got: '", spl[col_flux[b]], "'");
            }

            return false;
        }

        for (uint_t k : range(nfeature)) {
            input(gid,k) = flx[k];

            // Flag bad values
            if (!is_finite(flx[k])) {
                input(gid,k) = fnan;
            }
        }

        // Read input uncertainties
        if (opts.use_errors) {
            vec1f err;
            good = from_string(spl[col_eflux], err);
            if (count(!good) != 0) {
                for (uint_t b : where(!good)) {
                    error("could not read feature uncertainty (", header[col_eflux[b]], ") from line ", l);
                    note("must be a floating point number, got: '", spl[col_eflux[b]], "'");
                }

                return false;
            }

            for (uint_t k : range(nfeature)) {
                inputError(gid,k) = err[k];

                // Flag bad values
                if (!is_finite(err[k]) || err[k] < 0.0) {
                    input(gid,k) = fnan;
                    inputError(gid,k) = fnan;
                }
            }
        }

        // Read output
        if (col_output != npos) {
            if (!from_string(spl[col_output], output[gid])) {
                error("could not read output (", opts.output_column, ") from line ", l);
                note("must be a floating point number, got: '", spl[col_output], "'");
                return false;
            }

            // Remove excluded values
            if (output[gid] < opts.output_min || output[gid] > opts.output_max) {
                output[gid] = fnan;
            }
        }

        // Read weight
        if (col_weight != npos) {
            if (!from_string(spl[col_weight], weight[gid])) {
                error("could not read weight (", opts.weight_column, ") from line ", l);
                note("must be a floating point number, got: '", spl[col_weight], "'");
                return false;
            }
        }

        ++gid;
    }

    if (!opts.transform_inputs.empty()) {
        for (uint_t i : range(nfeature)) {
            if (opts.transform_inputs == "flux_to_luptitude") {
                double f0 = 0.0;
                if (opts.use_errors) {
                    vec1d tmp(ngal);
                    for (uint_t k : range(ngal)) tmp[k] = inputError(k,i);
                    f0 = inplace_median(tmp);
                } else {
                    vec1d tmp(ngal);
                    for (uint_t k : range(ngal)) tmp[k] = input(k,i);
                    tmp = tmp[where(tmp > 0)];
                    f0 = inplace_median(tmp);
                }

                for (uint_t k : range(ngal)) {
                    input(k,i) /= 2.0*f0;
                    if (opts.use_errors) {
                        inputError(k,i) = (2.5/log(10.0))*inputError(k,i)
                            /sqrt(1.0 + sqr(input(k,i)))/(2.0*f0);
                    }
                    input(k,i) = -(2.5/log(10.0))*(asinh(input(k,i)) + log(f0));
                }
            }
        }
    }

    return true;
}

bool read_training(options_t& opts,
    PHZ_GPz::Vec2d& input, PHZ_GPz::Vec2d& inputError,
    PHZ_GPz::Vec1d& output, PHZ_GPz::Vec1d& weight) {

    vec1s id;
    if (!read_ascii(opts, opts.training_catalog, id, input, inputError, output, weight, "training")) {
        return false;
    }

    return true;
}

bool read_prediction(options_t& opts,
    vec1s& id, PHZ_GPz::Vec2d& input, PHZ_GPz::Vec2d& inputError) {

    PHZ_GPz::Vec1d output, weight;
    if (!read_ascii(opts, opts.prediction_catalog, id, input, inputError, output, weight, "prediction")) {
        return false;
    }

    return true;
}
