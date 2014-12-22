#' Function for generating a series of outcomes related to a \code{dvhmatrix} class object
#' @description This function can be used for simulating outcome related to a given vector of doses or
#' a \code{dvhmatrix} class object after setting the outcome related parameters \eqn{TD_{50}}{TD50}, \eqn{\gamma_{50}}{\gamma 50}
#' and \eqn{a}. If \code{doses} isn't a \code{dvhmatrix} class object and it is a vector of nominal doses
#' the \code{a} parameter is ignored.
#' @param doses a \code{dvhmatrix} class object or a vector of nominal doses
#' @param a factor for calculating EUD value according \code{\link{DVH.eud}} function
#' @param TD50 Dose that gives the 50\% probability of outcome
#' @param gamma50 Slope of dose-response curve at TD50 of administered dose
#' @export
#' @return A vector of binary events (1 or 0).
DR.generate<-function(doses, a = 1, TD50 = 45, gamma50 = 1.5) {
  
}

## Dose/Response according Lyman 1985, Kutcher and Burman 1989, Deasy 2000 (probit) ##
#' Function that calculates NTCP according Lyman/Kutcher/Burman model
#' @description This function calculates the Normal Tissue Complication Probability according the
#' Lyman/Kutcher/Burman model. It is a reformulation of probit model by using other parameters than mean and standard deviation.
#' In its original formulation the NTCP is given by the following equation: \deqn{NTCP=1/\sqrt{2\pi}\int_{-\infty}^{t}exp(-x^2/2)dx}
#' where: \deqn{v=\frac{V}{V_{ref}}} \deqn{t=(D-TD_{50}(v))/(m*TD_{50}(v))} \deqn{TD(v)=TD(1)*v-n} In previous equations \eqn{V} is the
#' irradiated fraction volume and \eqn{V_{ref}} is the referenced volume for the given outcome, \eqn{m} is another way
#' to describe the slope of dose-response curve (see following lines).
#' In the \pkg{moddicom} implementation the parameters to be set in the model are \eqn{TD_{50}} and \eqn{\gamma_{50}}.
#' The model has been coded by adapting the original formula using these parameters in this way:
#' \deqn{t=(EUD-TD_{50})/(m*TD_{50})} with:
#' \deqn{m=\frac{1}{\gamma_{50}\sqrt{2\pi}}} The relationship between Dose and Volume has been achieved by using the
#' equivalent uniform dose as calculated by \code{\link{DVH.eud}} function.
#' @param doses Either a \code{dvhmatrix} class object or a vector with nominal doses
#' @param TD50 The value of dose that gives the 50\% of probability of outcome
#' @param gamma50 The slope of dose/response curve at 50\% of probability
#' @param a Value for parallel-serial correlation in radiobiological response
#' @export
#' @return A vector with NTCP(s) calculated according LKB model.
#' @references Burman C, Kutcher GJ, Emami B, Goitein M. \emph{Fitting of normal tissue tolerance data to an analytic function}. Int J Radiat Oncol Biol Phys. 1991 May 15;21(1):123-35. PubMed PMID: 2032883.
DR.Lyman <- function (doses, TD50 = 45, gamma50 = 1.5, a = 1) {
  if (class(doses)=="numeric") p <- pnorm(q=((doses - TD50)*gamma50*sqrt(2*pi))/TD50)
  if (class(doses)=="dvhmatrix") p <- pnorm(q=((DVH.eud(dvh = doses, a=a) - TD50)*gamma50*sqrt(2*pi))/TD50)
  return(p)
}

## Dose/Response according Goitein 1979 (logprobit) ##
#' Function that calculates NTCP according Goitein (Bentzen) model
#' @description This function calculates the Normal Tissue Complication Probability according the
#' Goitein (Bentzen) model. It is similar to Lyman but it is function of \emph{logarithm} of the Dose.
#' Starting from the implementation of Lyman model: \deqn{NTCP=1/\sqrt{2\pi}\int_{-\infty}^{t}exp(-x^2/2)dx}
#' where: \deqn{v=\frac{V}{V_{ref}}} \deqn{t=(log(D)-TD_{50}(v))/(m*TD_{50}(v))} \deqn{TD(v)=TD(1)*v-n} In previous equations \eqn{V} is the
#' irradiated fraction volume and \eqn{V_{ref}} is the referenced volume for the given outcome, \eqn{m} is another way
#' to describe the slope of dose-response curve (see following lines).
#' In the \pkg{moddicom} implementation the parameters to be set in the model are \eqn{TD_{50}} and \eqn{\gamma_{50}}.
#' The model has been coded by adapting the original formula using these parameters in this way:
#' \deqn{t=(log(EUD)-TD_{50})/(m*TD_{50})} with:
#' \deqn{m=\frac{1}{\gamma_{50}\sqrt{2\pi}}} The relationship between Dose and Volume has been achieved by using the
#' equivalent uniform dose as calculated by \code{\link{DVH.eud}} function.
#' @param doses Either a \code{dvhmatrix} class object or a vector with nominal doses
#' @param TD50 The value of dose that gives the 50\% of probability of outcome
#' @param gamma50 The slope of dose/response curve at 50\% of probability
#' @param a Value for parallel-serial correlation in radiobiological response
#' @export
#' @return A vector with NTCP(s) calculated according Goitein model.
#' @references Shipley WU, Tepper JE, Prout GR Jr, Verhey LJ, Mendiondo OA, Goitein M, Koehler AM, Suit HD. \emph{Proton radiation as boost therapy for localized prostatic carcinoma}. JAMA. 1979 May 4;241(18):1912-5. PubMed PMID: 107338.
#' @references Bentzen SM, Tucker SL. \emph{Quantifying the position and steepness of radiation dose-response curves}. Int J Radiat Biol. 1997 May;71(5):531-42. Review. PubMed PMID: 9191898.
DR.Goitein <- function (doses, TD50=45, gamma50=1.5, a=1) {
  if (class(doses)=="numeric") p <- pnorm(q=(log(doses/TD50)*gamma50*sqrt(2*pi)))
  if (class(doses)=="dvhmatrix") p <- pnorm(q=(log(DVH.eud(dvh = doses, a=a)/TD50)*gamma50*sqrt(2*pi)))
  return(p)
}

## Dose/Response according Niemierko 1991 (loglogit) ##
#' Function that calculates (N)TCP according Niemierko model
#' @description This function calculates the Normal Tissue Complication Probability according the
#' Niemierko model. This model can be used to compute Tumor Control Probability (TCP) too.
#' It is the translation of a \emph{loglogit} generalized linear model as function of \eqn{TD_{50}} and \eqn{\gamma_{50}}.
#' The model equation is: \deqn{(N)TCP=\frac{1}{1+ ({\frac{TD_{50}}{D}})^{4\gamma_{50}}}}
#' \eqn{D} can be either the nominal dose or the \eqn{EUD} as calculated by \code{\link{DVH.eud}} function.
#' @param doses Either a \code{dvhmatrix} class object or a vector with nominal doses
#' @param TD50 The value of dose that gives the 50\% of probability of outcome
#' @param gamma50 The slope of dose/response curve at 50\% of probability
#' @param a Value for parallel-serial correlation in radiobiological response 
#' @export
#' @return A vector with NTCP(s) calculated according Niemierko model.
#' @references Gay HA, Niemierko A. \emph{A free program for calculating EUD-based NTCP and TCP in external beam radiotherapy}. Phys Med. 2007 Dec;23(3-4):115-25. Epub 2007 Sep 7. PubMed PMID: 17825595.
DR.Niemierko <- function (doses, TD50=45, gamma50=1.5, a=1) {
  if (class(doses)=="numeric") p <- 1/(1+(TD50/doses)^(4*gamma50))
  if (class(doses)=="dvhmatrix") p <- 1/(1+(TD50/DVH.eud(dvh = doses, a=a))^(4*gamma50))
  return(p)
}

## Dose/Response according Munro, Gilbert, Kallman 1992 (Poisson approximation) ##
#' Function that calculates TCP according Munro/Gilbert/Kallman model
#' @description This function calculates the Tumor Control Probability according the
#' Munro/Gilbert/Kallman model. This model is an empyrical dose/response curve that fits experimental data. In their
#' paper authors assume this curve to be equivalent to a Poisson model. The original model equation is:
#' \deqn{TCP=e^{-EN_{0}e^{\frac{-D}{D_{0}}}}}
#' \eqn{E} is a numerical parameter that is related to tumor radiosensitivity, \eqn{N_{0}} is the total initial number of
#' tumor clonogenic cells, \eqn{D} is the delivered dose and \eqn{D_{0}} is the increment of dose that lowers survival 
#' to 37 per cent. In our implementation Munro/Gilbert/Kallman model has been referenced to \eqn{TD_{50}} and \eqn{\gamma_{50}} as follows:
#' \deqn{TCP=2^{e^{e\gamma_{50}(1-\frac{D}{TD_{50}})}}}
#' @param doses Either a \code{dvhmatrix} class object or a vector with nominal doses
#' @param TD50 The value of dose that gives the 50\% of probability of outcome
#' @param gamma50 The slope of dose/response curve at 50\% of probability
#' @param a Value for parallel-serial correlation in radiobiological response 
#' @export
#' @return A vector with TCP calculated according Munro/Gilbert/Kallman model.
#' @references Munro TR, Gilbert CW. \emph{The relation between tumour lethal doses and the radiosensitivity of tumour cells}. Br J Radiol. 1961 Apr;34:246-51. PubMed PMID: 13726846.
DR.Munro <- function (doses, TD50=45, gamma50=1.5, a=1) {
  if (class(doses)=="numeric") p <- 2^(-(exp(exp(1)*gamma50*(1-doses/TD50))))
  if (class(doses)=="dvhmatrix") p <- 2^(-(exp(exp(1)*gamma50*(1-DVH.eud(dvh = doses, a=a)/TD50))))
  return(p) 
}

## Dose/Response according Okunieff 1995 (logit) ##
#' Function that calculates TCP according Okunieff model
#' @description This function calculates the Tumor Control Probability according the
#' Okunieff model. This model is the equivalent of the \emph{logistic} generalized linear model where the covariates and their coefficients
#' have been reported as function of \eqn{TD_{50}} and \eqn{\gamma_{50}}. The original Okunieff formula is the following:
#' \deqn{TCP=\frac{e^{\frac{D-TD_{50}}{k}}}{1+e^{\frac{D-TD_{50}}{k}}}}
#' where \eqn{k=\gamma_{50}/(4*TD_{50})} and so giving the final model as direct function of \eqn{TD_{50}} and \eqn{\gamma_{50}}:
#' \deqn{TCP=\frac{1}{1+e^{4\gamma_{50}(1-\frac{D}{TD_{50}})}}}
#' @param doses Either a \code{dvhmatrix} class object or a vector with nominal doses
#' @param TD50 The value of dose that gives the 50\% of probability of outcome
#' @param gamma50 The slope of dose/response curve at 50\% of probability
#' @param a Value for parallel-serial correlation in radiobiological response 
#' @export
#' @return A vector with TCP calculated according Munro/Gilbert/Kallman model.
#' @references Okunieff P, Morgan D, Niemierko A, Suit HD. \emph{Radiation dose-response of human tumors}. Int J Radiat Oncol Biol Phys. 1995 Jul 15;32(4):1227-37. PubMed PMID: 7607946.
DR.Okunieff <- function (doses, TD50=45, gamma50=1.5, a=1) {
  if (class(doses)=="numeric") p <- 1/(1+exp(4*gamma50*(1-(doses/TD50))))
  if (class(doses)=="dvhmatrix") p <- 1/(1+exp(4*gamma50*(1-(DVH.eud(dvh = doses, a=a)/TD50))))
  return(p) 
}

## Dose/Response according Warkentin 2004 (Poisson) ##
DR.Warkentin <- function (TD50=45, gamma50=1.5, aa=1, diffdvh=NULL, dose=NULL) {
  # check the single choice between dvh matrix or dose series
  if (!is.null(dose) && !is.null(diffdvh)) stop("Select either a DVH or a point dose to calculate NTCP")
  # check the single choice between dvh matrix or dose series  
  if (is.vector(dose) && is.null(diffdvh)) p <- 0.5^(exp(2*gamma50/log(2)*(1-dose/TD50)))
  if (is.null(dose) && is.matrix(diffdvh)) p <- 0.5^(exp(2*gamma50/log(2)*(1-EUD(dvh.matrix=diffdvh, a=aa)/TD50)))
  return(p) 
}

## Dose/Response according Bentzen 1997 (log Poisson) ##
DR.Bentzen <- function (TD50=45, gamma50=1.5, aa=1, diffdvh=NULL, dose=NULL) {
  # check the single choice between dvh matrix or dose series
  if (!is.null(dose) && !is.null(diffdvh)) stop("Select either a DVH or a point dose to calculate NTCP")
  # check the single choice between dvh matrix or dose series  
  if (is.vector(dose) && is.null(diffdvh)) p <- 0.5^(TD50/dose)^(2*gamma50/log(2))
  if (is.null(dose) && is.matrix(diffdvh)) p <- 0.5^(TD50/EUD(dvh.matrix=diffdvh, a=aa))^(2*gamma50/log(2))
  return(p) 
}