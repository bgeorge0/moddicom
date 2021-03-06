% Generated by roxygen2 (4.1.1): do not edit by hand
% Please edit documentation in R/dvh_mod.R
\name{DVH.Dvolume}
\alias{DVH.Dvolume}
\title{Function for extracting the D-Volume from cumulative DVH(s)}
\usage{
DVH.Dvolume(dvh, Volume = 0.001)
}
\arguments{
\item{dvh}{A \code{dvhmatrix} class object}

\item{Volume}{Volume for calculating the D-Volume value(s). Default is 0.001, corresponding to about maximum dose
delivered to the structure.}
}
\value{
A vector containing the value(s) of D-Volume(s).
}
\description{
Function for calculating the value of D-Volume of a \code{dvhmatrix} object
}
\details{
D-Volume is, in a given Dose Volume Histogram, the value of the dose received
         by a given volume of the structure(s) of interest.
}
\examples{
## create a dvhmatrix class object
a<-DVH.generate(dvh.number = 100)
DVH.Dvolume(dvh = a, Volume = 0.5)
}

