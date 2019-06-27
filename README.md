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

## Lengths of Paths throught Polygons

The `castlemaine` data set can be used to generate paths via `dodgr`,
and the currently sole demo function can then take the polygon layer and
determine the relative length of the path that passes through the
polygons. Preliminary pre-processing, creating the `dodgr` street
network, and extracting the vertices to be used to generate a random
path.

``` r
devtools::load_all (".", export_all = FALSE)
#> Loading clipper
streetnet <- dodgr::weight_streetnet (castlemaine, wt_profile = "foot")
#> Loading required namespace: geodist
v <- dodgr::dodgr_vertices (streetnet)
v0 <- sample (v$id, size = 1)
v1 <- sample (v$id, size = 1)
p <- dodgr::dodgr_paths (streetnet, from = v0, to = v1)
p <- v [match (p [[1]] [[1]], v$id), ]
head (p)
#>             id        x         y component    n
#> 2061 402311218 144.2449 -37.07626         1 1938
#> 2060 402311217 144.2446 -37.07616         1 1937
#> 2059 402311216 144.2443 -37.07599         1 1936
#> 2058 402311215 144.2441 -37.07586         1 1935
#> 2057 402311214 144.2440 -37.07570         1 1934
#> 2056 402311213 144.2440 -37.07551         1 1933
```

We then just need to extract the coordinates of the polygon layer, and
ensure that the columns are labelled `x` and `y`:

``` r
green_polys <- castlemaine_green
green <- lapply (green_polys$geometry, function (i) {
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
#> [1] 0.1593343
```

And 15.9% of that path passes through the green polygons.
