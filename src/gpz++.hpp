#ifndef GPZPP_HPP
#define GPZPP_HPP

#include <vif/core/typedefs.hpp>
#include <vif/core/vec.hpp>
#include <vif/reflex/reflex.hpp>
#include <vif/core/error.hpp>
#include <vif/utility/generic.hpp>
#include <vif/utility/string.hpp>
#include <vif/utility/time.hpp>
#include <vif/math/base.hpp>
#include <vif/astro/astro.hpp>
#include <vif/io/ascii.hpp>
#include <iomanip>
#include <PHZ_GPz/GPz.h>

using namespace vif;
using namespace vif::astro;

extern const char* gpzpp_version;
extern const char* gpzpp_git_hash;

struct options_t {
    std::string training_catalog;
    std::string prediction_catalog;
    std::string output_catalog = "gpz.cat";
    std::string model_file = "gpz_model.dat";

    bool save_model = true;
    bool reuse_model = true;
    bool use_model_as_hint = false;

    std::string output_column = "z_spec";
    std::string weight_column = "";
    std::string flux_column_prefix = "F";
    std::string error_column_prefix = "E";
    bool        use_errors = true;
    vec1s       bands_regex;
    double      output_min = -finf;
    double      output_max = +finf;
    std::string transform_inputs = "";

    vec1s bands;
};

// Read inputs
bool read_config(const std::string& filename, options_t& opts, PHZ_GPz::GPz& gpz);

bool read_model(options_t& opts, PHZ_GPz::GPzModel& model);

bool read_training(options_t& opts,
    PHZ_GPz::Vec2d& input, PHZ_GPz::Vec2d& inputError,
    PHZ_GPz::Vec1d& output, PHZ_GPz::Vec1d& weight);

bool read_prediction(options_t& opts,
    vec1s& id, PHZ_GPz::Vec2d& input, PHZ_GPz::Vec2d& inputError);

// Write outputs
void write_model(const options_t& opts, const PHZ_GPz::GPzModel& model);

void write_output(const options_t& opts, const PHZ_GPz::GPz& gpz,
    const vec1s& id, const PHZ_GPz::GPzOutput& out);

#endif
