#include "clipper-rcpp.h"

// clipper only works with integers, so double values have to be multiplied by
// this amount before converting to int:
const long long mult = 1e6;

//' rcpp_areas
//'
//' layer A layer of polygons
//' @noRd
// [[Rcpp::export]]
Rcpp::NumericVector rcpp_areas (
        const Rcpp::List layer)
{
    const int n = layer.size ();

    Rcpp::NumericVector areas (n, 0.0);
    for (int i = 0; i < n; i++)
    {
        Rcpp::DataFrame li = Rcpp::as <Rcpp::DataFrame> (layer [i]);
        Rcpp::NumericVector lx = li ["x"], ly = li ["y"];
        ClipperLib::Path path;
        for (size_t j = 0; j < lx.size (); j++)
            path << ClipperLib::IntPoint (round (lx [j] * mult),
                    round (ly [j] * mult));

        areas [i] = fabs (ClipperLib::Area (path)) / (mult * mult);
    }

    return areas;
}

//' rcpp_path_in_poly
//'
//' @noRd
// [[Rcpp::export]]
double rcpp_path_in_poly (
        const Rcpp::List layer,
        const Rcpp::DataFrame path_in)
{
    const std::vector <double> x = path_in ["x"], y = path_in ["y"];

    ClipperLib::Paths path (1);
    double dtot = 0.0;
    int xold, yold;
    for (size_t i = 0; i < path_in.nrow (); i++)
    {
        int xi = round (x [i] * mult);
        int yi = round (y [i] * mult);
        path [0] << ClipperLib::IntPoint (xi, yi);
        if (i > 0)
        {
            double dx = static_cast <double> (xi - xold),
                   dy = static_cast <double> (yi - yold);
            dtot += sqrt (dx * dx + dy * dy);
        }
        xold = xi;
        yold = yi;
    }

    ClipperLib::Paths polys (layer.size ());
    for (int i = 0; i < layer.size (); i++)
    {
        Rcpp::DataFrame li = Rcpp::as <Rcpp::DataFrame> (layer [i]);
        Rcpp::NumericVector lx = li ["x"], ly = li ["y"];
        for (size_t j = 0; j < lx.size (); j++)
            polys [i] << ClipperLib::IntPoint (round (lx [j] * mult),
                    round (ly [j] * mult));
    }
    // First create union of polygons
    ClipperLib::Clipper c0;
    ClipperLib::Paths polys_u;
    c0.AddPaths (polys, ClipperLib::ptSubject, true);
    c0.Execute (ClipperLib::ctUnion, polys_u,
            ClipperLib::pftNonZero, ClipperLib::pftNonZero);

    // Then intersect that union with the paths
    ClipperLib::Clipper c;
    ClipperLib::PolyTree solution;
    c.AddPaths (path, ClipperLib::ptSubject, false); // closed = false!
    c.AddPaths (polys_u, ClipperLib::ptClip, true);
    c.Execute (ClipperLib::ctIntersection, solution,
            ClipperLib::pftNonZero, ClipperLib::pftNonZero);

    ClipperLib::Paths clipped_paths;
    ClipperLib::OpenPathsFromPolyTree (solution, clipped_paths);

    double result = 0.0;
    if (clipped_paths.size () > 0)
    {
        for (auto cp: clipped_paths)
        {
            // lazy explicit loop easier here
            for (int i = 1; i < cp.size (); i++)
            {
                double dx = static_cast <double> (cp [i].X - cp [i - 1].X),
                       dy = static_cast <double> (cp [i].Y - cp [i - 1].Y);
                result += sqrt (dx * dx + dy * dy);
            }
        }
    }

    return result / dtot;
}
