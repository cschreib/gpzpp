#include "GPz++.hpp"

template<typename T>
void write_vec1d(std::ofstream& fout, const T& vec) {
    uint_t nelem = vec.size();
    for (uint_t i : range(nelem)) {
        if (i != 0) fout << " ";
        fout << vec[i];
    }
    fout << std::endl;
}

void write_vec2d(std::ofstream& fout, const PHZ_GPz::Vec2d& vec) {
    uint_t nrow = vec.rows();
    uint_t ncol = vec.cols();
    for (uint_t i : range(nrow))
    for (uint_t j : range(ncol)) {
        if (j != 0) fout << " ";
        fout << vec(i,j);
        if (j == ncol - 1) fout << std::endl;
    }
}

void write_model(const options_t& opts, const PHZ_GPz::GPzModel& model) {
    std::ofstream fout(opts.model_file);

    uint_t nfeature = model.featureMean.size();
    uint_t nbasis = model.modelWeights.size();

    fout << "## GPz " << gpzpp_version << " model file\n";

    fout << "## internal data (do not edit)\n";
    fout << "# number of features\n";
    fout << nfeature << std::endl;
    fout << "# feature column name\n";
    write_vec1d(fout, opts.bands);
    fout << "# feature mean\n";
    write_vec1d(fout, model.featureMean);
    fout << "# feature sigma\n";
    write_vec1d(fout, model.featureSigma);
    fout << "# output mean\n";
    fout << model.outputMean << std::endl;

    fout << "## model parameters (can edit)\n";
    fout << "# num basis functions\n";
    fout << nbasis << std::endl;
    fout << "# BF weights\n";
    write_vec1d(fout, model.modelWeights);
    fout << "# BF priors\n";
    write_vec1d(fout, model.modelInputPrior);
    fout << "# BF log relevances\n";
    write_vec1d(fout, model.parameters.basisFunctionLogRelevances);
    fout << "# BF uncertainty weights\n";
    write_vec1d(fout, model.parameters.uncertaintyBasisWeights);
    fout << "# BF uncertainty log relevances\n";
    write_vec1d(fout, model.parameters.uncertaintyBasisLogRelevances);
    fout << "# log uncertainty constant\n";
    fout << model.parameters.logUncertaintyConstant << std::endl;
    fout << "# BF inverse covariance\n";
    write_vec2d(fout, model.modelInvCovariance);
    fout << "# BF positions\n";
    write_vec2d(fout, model.parameters.basisFunctionPositions);
    fout << "# BF covariances\n";
    for (uint_t i : range(nbasis)) {
        write_vec2d(fout, model.parameters.basisFunctionCovariances[i]);
    }
}

void write_output(const options_t& opts, const PHZ_GPz::GPz& gpz,
    const vec1s& id, const PHZ_GPz::GPzOutput& out) {

    std::ofstream fout(opts.output_catalog);

    if (std::string(gpzpp_git_hash).empty()) {
        fout << "# GPz version: " << gpzpp_version << std::endl;
    } else {
        fout << "# GPz version: " << gpzpp_version << "-" << gpzpp_git_hash << std::endl;
    }
    fout << "# Training catalog file:   " << opts.training_catalog << std::endl;
    fout << "# Prediction catalog file: " << opts.prediction_catalog << std::endl;
    fout << "# Number of features:         " << gpz.getNumberOfFeatures() << std::endl;
    fout << "# Number of basis functions:  " << gpz.getNumberOfBasisFunctions() << std::endl;
    switch (gpz.getPriorMeanFunction()) {
    case PHZ_GPz::PriorMeanFunction::ZERO:
        fout << "# Prior mean function:        ZERO" << std::endl; break;
    case PHZ_GPz::PriorMeanFunction::CONSTANT_PREPROCESS:
        fout << "# Prior mean function:        CONSTANT_PREPROCESS" << std::endl; break;
    case PHZ_GPz::PriorMeanFunction::LINEAR_PREPROCESS:
        fout << "# Prior mean function:        LINEAR_PREPROCESS" << std::endl; break;
    case PHZ_GPz::PriorMeanFunction::LINEAR_MARGINALIZE:
        fout << "# Prior mean function:        LINEAR_MARGINALIZE" << std::endl; break;
    }
    switch (gpz.getWeightingScheme()) {
    case PHZ_GPz::WeightingScheme::UNIFORM:
        fout << "# Weighting scheme:           UNIFORM" << std::endl; break;
    case PHZ_GPz::WeightingScheme::ONE_OVER_ONE_PLUS_OUTPUT:
        fout << "# Weighting scheme:           ONE_OVER_ONE_PLUS_OUTPUT" << std::endl; break;
    case PHZ_GPz::WeightingScheme::BALANCED:
        fout << "# Weighting scheme:           BALANCED" << std::endl; break;
    case PHZ_GPz::WeightingScheme::CUSTOM:
        fout << "# Weighting scheme:           CUSTOM" << std::endl; break;
    }
    switch (gpz.getNormalizationScheme()) {
    case PHZ_GPz::NormalizationScheme::NATURAL:
        fout << "# Input normalization scheme: NATURAL" << std::endl; break;
    case PHZ_GPz::NormalizationScheme::WHITEN:
        fout << "# Input normalization scheme: WHITEN" << std::endl; break;
    }
    switch (gpz.getCovarianceType()) {
    case PHZ_GPz::CovarianceType::GLOBAL_LENGTH:
        fout << "# Covariance type:            GLOBAL_LENGTH (GPGL)" << std::endl; break;
    case PHZ_GPz::CovarianceType::VARIABLE_LENGTH:
        fout << "# Covariance type:            VARIABLE_LENGTH (GPVL)" << std::endl; break;
    case PHZ_GPz::CovarianceType::GLOBAL_DIAGONAL:
        fout << "# Covariance type:            GLOBAL_DIAGONAL (GPGD)" << std::endl; break;
    case PHZ_GPz::CovarianceType::VARIABLE_DIAGONAL:
        fout << "# Covariance type:            VARIABLE_DIAGONAL (GPVD)" << std::endl; break;
    case PHZ_GPz::CovarianceType::GLOBAL_COVARIANCE:
        fout << "# Covariance type:            GLOBAL_COVARIANCE (GPGC)" << std::endl; break;
    case PHZ_GPz::CovarianceType::VARIABLE_COVARIANCE:
        fout << "# Covariance type:            VARIABLE_COVARIANCE (GPVC)" << std::endl; break;
    }
    switch (gpz.getOutputUncertaintyType()) {
    case PHZ_GPz::OutputUncertaintyType::UNIFORM:
        fout << "# Output uncertainty type:    UNIFORM" << std::endl; break;
    case PHZ_GPz::OutputUncertaintyType::INPUT_DEPENDENT:
        fout << "# Output uncertainty type:    INPUT_DEPENDENT" << std::endl; break;
    }

    fout << "#";

    uint_t id_width = 7;
    if (!id.empty()) {
        id_width = min(max(length(id))+1, id_width);
        fout << align_right("id", id_width);
    }

    uint_t value_width = 15;
    fout << align_right("value", value_width);
    fout << align_right("uncertainty", value_width);
    fout << align_right("var.density", value_width);
    fout << align_right("var.tr.noise", value_width);
    fout << align_right("var.in.noise", value_width);
    fout << std::endl;

    uint_t nelem = out.value.size();
    for (uint_t i : range(nelem)) {
        if (!id.empty()) {
            fout << std::setw(id_width) << id[i];
        }

        fout << std::setw(value_width) << std::scientific << out.value[i];
        fout << std::setw(value_width) << std::scientific << out.uncertainty[i];
        fout << std::setw(value_width) << std::scientific << out.varianceTrainDensity[i];
        fout << std::setw(value_width) << std::scientific << out.varianceTrainNoise[i];
        fout << std::setw(value_width) << std::scientific << out.varianceInputNoise[i];

        fout << "\n";
    }
}
