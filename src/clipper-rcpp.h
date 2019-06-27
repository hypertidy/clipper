#pragma once

#include <Rcpp.h>

#include "clipper.h"

Rcpp::NumericVector rcpp_areas (
        const Rcpp::List layer);

double rcpp_path_in_poly (
        const Rcpp::List layer,
        const Rcpp::DataFrame path);
