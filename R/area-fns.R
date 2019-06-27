#' poly_area
#' 
#' Approximate area of a lat-lon polygon in square metres
#' @param polys A list of `data.frame` objects, each of which has lon-lat
#' coordinates tracing a closed polygon.
#' @return Vector of areas in square metres
#' @export
poly_area <- function (polys)
{
    polys <- lapply (polys, function (i) {
                         d <- geodist::geodist (i)
                         xy <- data.frame (stats::cmdscale (d))
                         names (xy) <- c ("x", "y")
                         return (xy)   })
    rcpp_areas (polys)
}
