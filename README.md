<!-- README.md is generated from README.Rmd. Please edit that file -->

[![Build
Status](https://travis-ci.org/hypertidy/clipper.svg)](https://travis-ci.org/hypertidy/clipper)
[![AppVeyor Build
Status](https://ci.appveyor.com/api/projects/status/github/hypertidy/clipper?branch=master&svg=true)](https://ci.appveyor.com/project/hypertidy/clipper)
[![codecov](https://codecov.io/gh/hypertidy/clipper/branch/master/graph/badge.svg)](https://codecov.io/gh/hypertidy/clipper)
[![Project Status:
Concept](http://www.repostatus.org/badges/latest/concept.svg)](http://www.repostatus.org/#concept)

# clipper: Another Port of Angus Johnson’s [‘clipper’ Polygon Clipping Library](http://angusj.com/delphi/clipper.php)

A few scribbles extending beyond CRAN package
[`polyclip`](https://cran.r-project.org/package=polyclip) to use
`clipper` for a few commonly used manipulations. Package has two sets of
internal data to play with:

1.  `castlemaine` - An `SC` representation of the street network of
    Castlemaine, Australia.
2.  `castlemaine_green` - An `sf::sfc` object containing all the
    polygons of green areas (parks, paddocks, lawns, forests) in
    eponymous location.

<!-- end list -->

``` r
devtools::load_all (".", export_all = FALSE)
#> Loading clipper
```

## Polygon Areas

The `clipper` package offers an alternative way to *approximately*
calculate the areas of polygons expressed in lat-lon values. The
following code demonstrates with the above polygon data, and compares
the result with equivalent areas calculated via `sf`.

``` r
# extract coordinates from sf polygon objects:
polys <- lapply (castlemaine_green$geometry, function (i) i [[1]])
a <- poly_area (polys)
a0 <- as.numeric (sf::st_area (castlemaine_green))
#> Linking to GEOS 3.7.1, GDAL 2.4.1, PROJ 5.2.0
mean ((a - a0) / a0)
#> [1] -0.001506411
```

The areas generally underestimate the actual values by about 1.5%. The
benchmark’s aren’t good either:

``` r
rbenchmark::benchmark (a <- poly_area (polys),
                       a0 <- sf::st_area (castlemaine_green),
                       replications = 10)
#>                                   test replications elapsed relative
#> 1                a <- poly_area(polys)           10   0.122    4.067
#> 2 a0 <- sf::st_area(castlemaine_green)           10   0.030    1.000
#>   user.self sys.self user.child sys.child
#> 1     0.118    0.004          0         0
#> 2     0.030    0.000          0         0
```

But the point of this function is when inputs are pure lon-lat
coordinates, *not* in `sf` format, in which case the benchmarks look a
little different.

``` r
poly_area_sf <- function (p)
{
     sf::st_polygon (list (p)) %>%
         sf::st_sfc (crs = 4326) %>%
         sf::st_area () %>%
         as.numeric ()
}

rbenchmark::benchmark (a <- poly_area (polys),
                       a0 <- unlist (lapply (polys, poly_area_sf)),
                       replications = 10)
#>                                        test replications elapsed relative
#> 1                     a <- poly_area(polys)           10   0.117    1.000
#> 2 a0 <- unlist(lapply(polys, poly_area_sf))           10   0.781    6.675
#>   user.self sys.self user.child sys.child
#> 1     0.117        0          0         0
#> 2     0.780        0          0         0
```

## Lengths of Paths through Polygons

The `castlemaine` data set can be used to generate paths via `dodgr`,
and the currently sole demo function can then take the polygon layer and
determine the relative length of the path that passes through the
polygons. Preliminary pre-processing, creating the `dodgr` street
network, and extracting the vertices to be used to generate a random
path.

``` r
streetnet <- dodgr::weight_streetnet (castlemaine, wt_profile = "foot")
v <- dodgr::dodgr_vertices (streetnet)
v0 <- sample (v$id, size = 1)
v1 <- sample (v$id, size = 1)
p <- dodgr::dodgr_paths (streetnet, from = v0, to = v1)
p <- v [match (p [[1]] [[1]], v$id), ]
head (p)
#>              id        x         y component    n
#> 7014 4650502476 144.1992 -37.03910         1 6532
#> 7013 4650502462 144.1991 -37.03905         1 6531
#> 7012 4650502478 144.1991 -37.03909         1 6530
#> 7011 4650503192 144.1992 -37.03922         1 6529
#> 7010 4650502487 144.1991 -37.03928         1 6528
#> 7009 4650502474 144.1990 -37.03929         1 6527
```

We then just need to extract the coordinates of the polygon layer, and
ensure that the columns are labelled `x` and `y`:

``` r
green <- lapply (castlemaine_green$geometry, function (i) {
                     res <- i [[1]]
                     colnames (res) <- c ("x", "y")
                     return (res)   })
length (green)
#> [1] 38
head (green [[1]])
#>                   x         y
#> 294072372  144.2126 -37.05438
#> 1977730380 144.2136 -37.05443
#> 294072377  144.2138 -37.05449
#> 294072383  144.2139 -37.05463
#> 294072393  144.2131 -37.05552
#> 294072398  144.2124 -37.05543
```

Then we’re good to go …

``` r
path_through_poly (p, green)
#> [1] 0.1274177
```

And 12.7% of that path passes through the green polygons.
