% Generated by roxygen2 (4.1.1): do not edit by hand
% Please edit documentation in R/mmButo.R
\name{RAD.getAttribute}
\alias{RAD.getAttribute}
\title{RAD.getAttribute - a wrapper function to retrieve an attribute from a mmButo object}
\usage{
RAD.getAttribute(obj, attribute, errorHandlerParams = c())
}
\arguments{
\item{obj}{an \code{mmButo} object}

\item{errorHandlerParams}{is a list to indicate what to do in case of error caught by moddicom (the error caught from R are not managed). The list can be so populated:
\itemize{
  \item \code{onFilePar} if set to \code{TRUE} the error/log messages will be written on a file;
  \item \code{fileName} if \code{onFilePar} is set to \code{TRUE} this attribute indicate the fileName. If it is not specificed the default is './defLogHandler.txt';
  \item \code{onScreenPar} if set to \code{TRUE} the error/log messages will be prompt on the screen;
  \item \code{returnOnEOL} if set to \code{TRUE} once the error is written, an End Of Line is added at the end of line.
}}

\item{attributeName}{a string with the name of the desired attribute:
\itemize{
  \item \code{dataStorage}  contains all the loaded data (images, DICOM heder information, etc...)
  \item \code{results} contains all the computed data. If no computation has been executed, no results will be returned in this attribute;
}}
}
\value{
The desired attribute, normally in form of \code{list}
}
\description{
retrieve an attribute from a \code{mmButo} object
}
\details{
This is just a wrapper of the method \code{getAttribute} defined in the class \code{mmButo}
}

