#' path_through_poly
#' 
#' Relative length of path that passes through polygons
#' @param path A `data.frame` with `x` and `y` columns denoting sequential
#'      coordinates of a path.
#' @param polys A list of `data.frame` objects, each of which has coordinates
#'      labelled `x` and `y` tracing a closed polygon.
#' @return Single value quantifying the total relative length of the input path
#'      that passes through the polygons
#' @export
path_through_poly <- function (path, polys)
{
    rcpp_path_in_poly (green, path)
}
