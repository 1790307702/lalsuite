/************************* <lalVerbatim file="SimulateTaylorCWTestCV">
Author: Creighton, T. D.
$Id$
**************************************************** </lalVerbatim> */

/********************************************************** <lalLaTeX>

\subsection{Program \texttt{SimulateTaylorCWTest.c}}
\label{ss:SimulateTaylorCWTest.c}

Generates a quasiperiodic waveform.

\subsubsection*{Usage}
\begin{verbatim}
SimulateTaylorCWTest [-s sourcefile] [-r respfile] [-l site earthfile sunfile]
                     [-o outfile] [-t sec nsec npt dt] [-d debuglevel]
\end{verbatim}

\subsubsection*{Description}

This program generates a Taylor-parameterized continuous waveform as a
function of time.  The following option flags are accepted:
\begin{itemize}
\item[\texttt{-s}] Reads source information from the file
\verb@sourcefile@, whose format is specified below.  If absent, it
injects a monochromatic wave with intrinsic frequency 100~Hz, strain
amplitude 1000, and zero phase at the GPS epoch, arriving from the
direction RA=00h00m, dec=$0^\circ$.
\item[\texttt{-r}] Reads a detector response function from the file
\verb@respfile@, whose format is specified below.  If absent, it
generates raw dimensionless strain.
\item[\texttt{-l}] Sets the detector location and orientation.
\verb@site@ must be one of the following character strings:
\verb@LHO@, \verb@LLO@, \verb@VIRGO@, \verb@GEO600@, \verb@TAMA300@,
or \verb@CIT40@.  \verb@earthfile@ and \verb@sunfile@ are ephemeris
files of the format expected by \verb@LALInitBarycenter()@.  If the
\verb@-l@ option is not specified, a stationary (barycentric) detector
aligned with the wave's + polarization is assumed.
\item[\texttt{-o}] Writes the generated time series to the file
\verb@outfile@.  If absent, the routines are exercised, but no output
is written.
\item[\texttt{-t}] Sets the timing information for the generated
waveform: \verb@sec@ and \verb@nsec@ are integers specifying the start
time in GPS seconds and nanoseconds, \verb@npt@ is the number of time
samples generated, and \verb@dt@ is the sampling interval in seconds.
If absent, \verb@-t 0 0 1048576 9.765625e-4 0.0@ is assumed.
\item[\texttt{-d}] Sets the debug level to \verb@debuglevel@.  If
absent, level 0 is assumed.
\end{itemize}

\paragraph{Format for \texttt{sourcefile}:} The source file consists
of any number of lines of data, each specifying a continuous waveform.
Each line consists of 8 or more whitespace-delimited numerical fields:
the GPS epoch where the frequency, phase, and Taylor coefficients are
defined (\verb@INT8@ nanoseconds), the + and $\times$ wave amplitudes
(\verb@REAL4@ strain) and polarization angle $\psi$ (\verb@REAL4@
radians), the right ascension and declination (\verb@REAL4@ degrees),
the initial frequency (\verb@REAL4@ Hz) and phase (\verb@REAL4@
degrees), followed by zero or more Taylor coefficients $f_k$
(\verb@REAL4@ Hz${}^k$).

\paragraph{Format for \texttt{respfile}:} The response function $R(f)$
gives the real and imaginary components of the transformation
\emph{from} ADC output $o$ \emph{to} tidal strain $h$ via
$\tilde{h}(f)=R(f)\tilde{o}(f)$.  It is inverted internally to give
the detector \emph{transfer function} $T(f)=1/R(f)$.  The format
\verb@respfile@ is a header specifying the GPS epoch $t_0$ at which
the response was taken (\verb@INT8@ nanoseconds), the lowest frequency
$f_0$ at which the response is given (\verb@REAL8@ Hz), and the
frequency sampling interval $\Delta f$ (\verb@REAL8@ Hz):

\medskip
\begin{tabular}{l}
\verb@# epoch = @$t_0$ \\
\verb@# f0 = @$f_0$ \\
\verb@# deltaF = @$\Delta f$
\end{tabular}
\medskip

\noindent followed by two columns of \verb@REAL4@ data giving the real
and imaginary components of $R(f_0+k\Delta f)$.

\paragraph{Format for \texttt{outfile}:} The ouput files generated by
this program consist of a two-line header giving the GPS epoch $t_0$
of the first time sample (\verb@INT8@ nanoseconds) and the sampling
interval $\Delta t$ (\verb@REAL8@ seconds):

\medskip
\begin{tabular}{l}
\verb@# epoch = @$t_0$ \\
\verb@# deltaT = @$\Delta t$
\end{tabular}
\medskip

\noindent followed by a single column of \verb@REAL4@ data
representing the undigitized output of the interferometer's
gravitational-wave channel.

\subsubsection*{Exit codes}
****************************************** </lalLaTeX><lalErrTable> */
#define SIMULATETAYLORCWTESTC_ENORM  0
#define SIMULATETAYLORCWTESTC_ESUB   1
#define SIMULATETAYLORCWTESTC_EARG   2
#define SIMULATETAYLORCWTESTC_EVAL   3
#define SIMULATETAYLORCWTESTC_EFILE  4
#define SIMULATETAYLORCWTESTC_EINPUT 5
#define SIMULATETAYLORCWTESTC_EMEM   6

#define SIMULATETAYLORCWTESTC_MSGENORM  "Normal exit"
#define SIMULATETAYLORCWTESTC_MSGESUB   "Subroutine failed"
#define SIMULATETAYLORCWTESTC_MSGEARG   "Error parsing arguments"
#define SIMULATETAYLORCWTESTC_MSGEVAL   "Input argument out of valid range"
#define SIMULATETAYLORCWTESTC_MSGEFILE  "Could not open file"
#define SIMULATETAYLORCWTESTC_MSGEINPUT "Error reading file"
#define SIMULATETAYLORCWTESTC_MSGEMEM   "Out of memory"
/******************************************** </lalErrTable><lalLaTeX>

\subsubsection*{Algorithm}

If a \verb@sourcefile@ is specified, this program first reads the
epoch field, and then calls \verb@LALSReadVector()@ to read the
remaining fields.  (If no \verb@sourcefile@ is specified, the
parameters are taken from \verb@#define@d constants.) The arguments
are passed to \verb@LALGenerateTaylorCW()@ to generate frequency and
phase timeseries.  The required sampling resolution for these
timeseries is estimated at $0.1/\sum_k\sqrt{|kf_0f_kT^{k-1}|}$, where
$T$ is the duration of the waveform, to ensure that later
interpolation of the waveforms will give phases accurate to well
within a radian.

The output from \verb@LALGenerateTaylorCW()@ is then passed to
\verb@LALSimulateCoherentGW()@ to generate an output time series.  If
there are multiple lines in \verb@sourcefile@, the procedure is
repeated and the new waveforms added into the output.  A warning is
generated if the wave frequency exceeds the Nyquist rate for the
output time series.

\subsubsection*{Uses}
\begin{verbatim}
lalDebugLevel
LALPrintError()                 LALCheckMemoryLeaks()
LALSCreateVector()              LALSDestroyVector()
LALGenerateTaylorCW()        LALSDestroyVectorSequence()
\end{verbatim}

\subsubsection*{Notes}

\vfill{\footnotesize\input{SimulateTaylorCWTestCV}}

******************************************************* </lalLaTeX> */

#include <math.h>
#include <stdlib.h>
#include <lal/LALStdio.h>
#include <lal/LALStdlib.h>
#include <lal/LALConstants.h>
#include <lal/StreamInput.h>
#include <lal/AVFactories.h>
#include <lal/SeqFactories.h>
#include <lal/VectorOps.h>
#include <lal/Units.h>
#include <lal/SimulateCoherentGW.h>
#include <lal/GenerateTaylorCW.h>
#include <lal/LALBarycenter.h>
#include "LALInitBarycenter.h"

NRCSID( SIMULATETAYLORCWTESTC, "$Id$" );

/* Default parameter settings. */
int lalDebugLevel = 0;
#define EPOCH  (0LL) /* about Jan. 1, 1990 */
#define APLUS  (1000.0)
#define ACROSS (1000.0)
#define RA     (0.0)
#define DEC    (0.0)
#define PSI    (0.0)
#define F0     (100.0)
#define PHI0   (0.0)
#define SEC    (0)
#define NSEC   (0)
#define DT     (0.0009765625)
#define NPT    (1048576) 

/* Usage format string. */
#define USAGE "Usage: %s [-s sourcefile] [-o outfile]\n"             \
"\t[-r respfile] [-l site earthfile sunfile]\n"                      \
"\t[-t sec nsec npt dt] [-d debuglevel]\n"

/* Maximum output message length. */
#define MSGLEN (1024)

/* Upper cutoff frequency for default detector response function. */
#define FSTOP (16384.0)

/* Macros for printing errors and testing subroutines. */
#define ERROR( code, msg, statement )                                \
do                                                                   \
if ( lalDebugLevel & LALERROR )                                      \
{                                                                    \
  LALPrintError( "Error[0] %d: program %s, file %s, line %d, %s\n"   \
		 "        %s %s\n", (code), *argv, __FILE__,         \
		 __LINE__, SIMULATETAYLORCWTESTC,                    \
		 statement ? statement : "", (msg) );                \
}                                                                    \
while (0)

#define INFO( statement )                                            \
do                                                                   \
if ( lalDebugLevel & LALINFO )                                       \
{                                                                    \
  LALPrintError( "Info[0]: program %s, file %s, line %d, %s\n"       \
		 "        %s\n", *argv, __FILE__, __LINE__,          \
		 SIMULATETAYLORCWTESTC, (statement) );               \
}                                                                    \
while (0)

#define WARNING( statement )                                         \
do                                                                   \
if ( lalDebugLevel & LALWARNING )                                    \
{                                                                    \
  LALPrintError( "Warning[0]: program %s, file %s, line %d, %s\n"    \
		 "        %s\n", *argv, __FILE__, __LINE__,          \
		 SIMULATETAYLORCWTESTC, (statement) );               \
}                                                                    \
while (0)

#define SUB( func, statusptr )                                       \
do                                                                   \
if ( (func), (statusptr)->statusCode )                               \
{                                                                    \
  ERROR( SIMULATETAYLORCWTESTC_ESUB, SIMULATETAYLORCWTESTC_MSGESUB,  \
	 "Function call \"" #func "\" failed:" );                    \
  return SIMULATETAYLORCWTESTC_ESUB;                                 \
}                                                                    \
while (0)

#define CHECKVAL( val, lower, upper )                                \
do                                                                   \
if ( ( (val) < (lower) ) || ( (val) > (upper) ) )                    \
{                                                                    \
  ERROR( SIMULATETAYLORCWTESTC_EVAL, SIMULATETAYLORCWTESTC_MSGEVAL,  \
         "Value of " #val " out of range:" );                        \
  if ( lalDebugLevel & LALERROR )                                    \
    LALPrintError( #val " = %f, range = [%f,%f]\n", (REAL8)(val),    \
                   (REAL8)(lower), (REAL8)(upper) );                 \
  return SIMULATETAYLORCWTESTC_EVAL;                                 \
}                                                                    \
while (0)

/* A global pointer for debugging. */
#ifndef NDEBUG
char *lalWatch;
#endif

/* A function to convert INT8 nanoseconds to LIGOTimeGPS. */
void
I8ToLIGOTimeGPS( LIGOTimeGPS *output, INT8 input );

/* A function to compute the combination function C(a+1,b+1). */
UINT4
choose( UINT4 a, UINT4 b );


int
main(int argc, char **argv)
{
  /* Command-line parsing variables. */
  int arg;                     /* command-line argument counter */
  static LALStatus stat;       /* status structure */
  CHAR *sourcefile = NULL;     /* name of sourcefile */
  CHAR *respfile = NULL;       /* name of respfile */
  CHAR *outfile = NULL;        /* name of outfile */
  CHAR *earthfile = NULL;      /* name of earthfile */
  CHAR *sunfile = NULL;        /* name of sunfile */
  CHAR *site = NULL;           /* name of detector site */
  INT4 npt = NPT;              /* number of output samples */
  INT4 sec = SEC, nsec = NSEC; /* GPS epoch of output */
  REAL8 dt = DT;               /* output sampling interval */

  /* File reading variables. */
  FILE *fp = NULL;    /* generic file pointer */
  BOOLEAN ok = 1;     /* whether input format is correct */
  INT8 epoch = EPOCH; /* epoch stored as an INT8 */

  /* Other variables. */
  UINT4 i, j;                /* generic indecies */
  INT8 tStart, tStop;        /* start and stop times for waveform */
  DetectorResponse detector; /* the detector in question */
  REAL4TimeSeries output;    /* detector output */

  /*******************************************************************
   * ARGUMENT PARSING (arg stores the current position)              *
   *******************************************************************/

  arg = 1;
  while ( arg < argc ) {
    /* Parse source file option. */
    if ( !strcmp( argv[arg], "-s" ) ) {
      if ( argc > arg + 1 ) {
	arg++;
	sourcefile = argv[arg++];
      } else {
	ERROR( SIMULATETAYLORCWTESTC_EARG,
	       SIMULATETAYLORCWTESTC_MSGEARG, 0 );
        LALPrintError( USAGE, *argv );
        return SIMULATETAYLORCWTESTC_EARG;
      }
    }
    /* Parse response file option. */
    else if ( !strcmp( argv[arg], "-r" ) ) {
      if ( argc > arg + 1 ) {
	arg++;
	respfile = argv[arg++];
      } else {
	ERROR( SIMULATETAYLORCWTESTC_EARG,
	       SIMULATETAYLORCWTESTC_MSGEARG, 0 );
        LALPrintError( USAGE, *argv );
        return SIMULATETAYLORCWTESTC_EARG;
      }
    }
    /* Parse output file option. */
    else if ( !strcmp( argv[arg], "-o" ) ) {
      if ( argc > arg + 1 ) {
	arg++;
	outfile = argv[arg++];
      } else {
	ERROR( SIMULATETAYLORCWTESTC_EARG,
	       SIMULATETAYLORCWTESTC_MSGEARG, 0 );
        LALPrintError( USAGE, *argv );
        return SIMULATETAYLORCWTESTC_EARG;
      }
    }
    /* Parse detector location option. */
    else if ( !strcmp( argv[arg], "-l" ) ) {
      if ( argc > arg + 3 ) {
	arg++;
	site = argv[arg++];
	earthfile = argv[arg++];
	sunfile = argv[arg++];
      } else {
	ERROR( SIMULATETAYLORCWTESTC_EARG,
	       SIMULATETAYLORCWTESTC_MSGEARG, 0 );
        LALPrintError( USAGE, *argv );
        return SIMULATETAYLORCWTESTC_EARG;
      }
    }
    /* Parse output timing option. */
    else if ( !strcmp( argv[arg], "-t" ) ) {
      if ( argc > arg + 4 ) {
	arg++;
	sec = atoi( argv[arg++] );
	nsec = atoi( argv[arg++] );
	npt = atoi( argv[arg++] );
	dt = atof( argv[arg++] );
      } else {
	ERROR( SIMULATETAYLORCWTESTC_EARG,
	       SIMULATETAYLORCWTESTC_MSGEARG, 0 );
        LALPrintError( USAGE, *argv );
        return SIMULATETAYLORCWTESTC_EARG;
      }
    }
    /* Parse debug level option. */
    else if ( !strcmp( argv[arg], "-d" ) ) {
      if ( argc > arg + 1 ) {
	arg++;
	lalDebugLevel = atoi( argv[arg++] );
      }else{
	ERROR( SIMULATETAYLORCWTESTC_EARG,
	       SIMULATETAYLORCWTESTC_MSGEARG, 0 );
        LALPrintError( USAGE, *argv );
        return SIMULATETAYLORCWTESTC_EARG;
      }
    }
    /* Check for unrecognized options. */
    else if ( argv[arg][0] == '-' ) {
      ERROR( SIMULATETAYLORCWTESTC_EARG,
	     SIMULATETAYLORCWTESTC_MSGEARG, 0 );
      LALPrintError( USAGE, *argv );
      return SIMULATETAYLORCWTESTC_EARG;
    }
  } /* End of argument parsing loop. */

  /* Make sure that values won't crash the system or anything. */
  CHECKVAL( dt, LAL_REAL4_MIN, LAL_REAL4_MAX );
  CHECKVAL( npt, 0, 2147483647 );


  /*******************************************************************
   * SETUP                                                           *
   *******************************************************************/

  /* Set up output structure and wave start and stop times. */
  {
    INT8 epochOut = nsec;    /* output epoch */
    epochOut += 1000000000LL*sec;
    tStart = epochOut - 1000000000LL;
    memset( &output, 0, sizeof(REAL4TimeSeries) );
    I8ToLIGOTimeGPS( &(output.epoch), epochOut );
    output.deltaT = dt;
    LALSnprintf( output.name, LALNameLength, "Taylor CW waveform" );
    SUB( LALSCreateVector( &stat, &(output.data), npt ), &stat );
    memset( output.data->data, 0, npt*sizeof(REAL4) );
    tStop = epochOut + 1000000000LL*(INT8)( dt*npt + 1.0 );
  }
  /* Adjust wave start and stop times so that they will almost
     certainly cover the output timespan even after barycentring. */
  if ( site ) {
    tStart -= (INT8)( (1.1e9)*LAL_AU_SI/LAL_C_SI );
    tStop += (INT8)( (1.1e9)*LAL_AU_SI/LAL_C_SI );
  }


  /* Set up detector structure. */
  memset( &detector, 0, sizeof(DetectorResponse) );
  detector.transfer = (COMPLEX8FrequencySeries *)
    LALMalloc( sizeof(COMPLEX8FrequencySeries) );
  if ( !(detector.transfer) ) {
    ERROR( SIMULATETAYLORCWTESTC_EMEM,
	   SIMULATETAYLORCWTESTC_MSGEMEM, 0 );
    return SIMULATETAYLORCWTESTC_EMEM;
  }
  memset( detector.transfer, 0, sizeof(COMPLEX8FrequencySeries) );
  if ( respfile ) {
    REAL4VectorSequence *resp = NULL; /* response as vector sequence */
    COMPLEX8Vector *response = NULL;  /* response as complex vector */
    COMPLEX8Vector *unity = NULL;     /* vector of complex 1's */

    if ( ( fp = fopen( respfile, "r" ) ) == NULL ) {
      ERROR( SIMULATETAYLORCWTESTC_EFILE,
	     SIMULATETAYLORCWTESTC_MSGEFILE, respfile );
      return SIMULATETAYLORCWTESTC_EFILE;
    }

    /* Read header. */
    ok &= ( fscanf( fp, "# epoch = %lli\n", &epoch ) == 1 );
    I8ToLIGOTimeGPS( &( detector.transfer->epoch ), epoch );
    ok &= ( fscanf( fp, "# f0 = %lf\n", &( detector.transfer->f0 ) )
            == 1 );
    ok &= ( fscanf( fp, "# deltaF = %lf\n",
                    &( detector.transfer->deltaF ) ) == 1 );
    if ( !ok ) {
      ERROR( SIMULATETAYLORCWTESTC_EINPUT,
	     SIMULATETAYLORCWTESTC_MSGEINPUT, respfile );
      return SIMULATETAYLORCWTESTC_EINPUT;
    }

    /* Read and convert body to a COMPLEX8Vector. */
    SUB( LALSReadVectorSequence( &stat, &resp, fp ), &stat );
    fclose( fp );
    if ( resp->vectorLength != 2 ) {
      ERROR( SIMULATETAYLORCWTESTC_EINPUT,
	     SIMULATETAYLORCWTESTC_MSGEINPUT, respfile );
      return SIMULATETAYLORCWTESTC_EINPUT;
    }
    SUB( LALCCreateVector( &stat, &response, resp->length ), &stat );
    memcpy( response->data, resp->data, 2*resp->length*sizeof(REAL4) );
    SUB( LALSDestroyVectorSequence( &stat, &resp ), &stat );

    /* Convert response function to a transfer function. */
    SUB( LALCCreateVector( &stat, &unity, response->length ), &stat );
    for ( i = 0; i < response->length; i++ ) {
      unity->data[i].re = 1.0;
      unity->data[i].im = 0.0;
    }
    SUB( LALCCreateVector( &stat, &( detector.transfer->data ),
                           response->length ), &stat );
    SUB( LALCCVectorDivide( &stat, detector.transfer->data, unity,
                            response ), &stat );
    SUB( LALCDestroyVector( &stat, &response ), &stat );
    SUB( LALCDestroyVector( &stat, &unity ), &stat );
  }
  /* No response file, so generate a unit response. */
  else {
    I8ToLIGOTimeGPS( &( detector.transfer->epoch ), EPOCH );
    detector.transfer->f0 = 0.0;
    detector.transfer->deltaF = FSTOP;
    SUB( LALCCreateVector( &stat, &( detector.transfer->data ), 2 ),
         &stat );
    detector.transfer->data->data[0].re = 1.0;
    detector.transfer->data->data[1].re = 1.0;
    detector.transfer->data->data[0].im = 0.0;
    detector.transfer->data->data[1].im = 0.0;
  }
  if ( site ) {
    /* Set detector location. */
    if ( !strcmp( site, "LHO" ) )
      i = LALDetectorIndexLHODIFF;
    else if ( !strcmp( site, "LLO" ) )
      i = LALDetectorIndexLLODIFF;
    else if ( !strcmp( site, "VIRGO" ) )
      i = LALDetectorIndexVIRGODIFF;
    else if ( !strcmp( site, "GEO600" ) )
      i = LALDetectorIndexGEO600DIFF;
    else if ( !strcmp( site, "TAMA300" ) )
      i = LALDetectorIndexTAMA300DIFF;
    else if ( !strcmp( site, "CIT40" ) )
      i = LALDetectorIndexCIT40DIFF;
    else {
      ERROR( SIMULATETAYLORCWTESTC_EVAL,
	     SIMULATETAYLORCWTESTC_MSGEVAL, "Unrecognized site:" );
      if ( lalDebugLevel & LALERROR )
	LALPrintError( "%s", site );
      return SIMULATETAYLORCWTESTC_EVAL;
    }
    detector.site = (LALDetector *)LALMalloc( sizeof(LALDetector) );
    if ( !(detector.site) ) {
      ERROR( SIMULATETAYLORCWTESTC_EMEM,
	     SIMULATETAYLORCWTESTC_MSGEMEM, 0 );
      return SIMULATETAYLORCWTESTC_EMEM;
    }
    *(detector.site) = lalCachedDetectors[i];

    /* Read ephemerides. */
    detector.ephemerides = (EphemerisData *)
      LALMalloc( sizeof(EphemerisData) );
    if ( !(detector.ephemerides) ) {
      ERROR( SIMULATETAYLORCWTESTC_EMEM,
	     SIMULATETAYLORCWTESTC_MSGEMEM, 0 );
      return SIMULATETAYLORCWTESTC_EMEM;
    }
    detector.ephemerides->ephiles.earthEphemeris = earthfile;
    detector.ephemerides->ephiles.sunEphemeris = sunfile;
    SUB( LALInitBarycenter( &stat, detector.ephemerides ), &stat );
  }


  /* Set up units for the above structures. */
  {
    RAT4 negOne = { -1, 0 }; /* -1 as a rational number */
    LALUnitPair pair;        /* structure for defining response units */
    output.sampleUnits = pair.unitOne = lalADCCountUnit;
    pair.unitTwo = lalStrainUnit;
    SUB( LALUnitRaise( &stat, &(pair.unitTwo), &(pair.unitTwo),
                       &negOne ), &stat );
    SUB( LALUnitMultiply( &stat, &(detector.transfer->sampleUnits),
                          &pair ), &stat );
  }


  /*******************************************************************
   * OUTPUT GENERATION                                               *
   *******************************************************************/

  /* Open sourcefile. */
  if ( sourcefile ) {
    if ( ( fp = fopen( sourcefile, "r" ) ) == NULL ) {
      ERROR( SIMULATETAYLORCWTESTC_EFILE,
	     SIMULATETAYLORCWTESTC_MSGEFILE, sourcefile );
      return SIMULATETAYLORCWTESTC_EFILE;
    }
  }

  /* For each line in the sourcefile... */
  while ( ok ) {
    TaylorCWParamStruc params; /* wave generation parameters */
    CoherentGW waveform;       /* amplitude and phase structure */
    REAL4TimeSeries signal;    /* GW signal */
    CHAR message[MSGLEN];      /* warning/info messages */

    /* Initialize output structures. */
    memset( &waveform, 0, sizeof(CoherentGW) );
    signal = output;
    signal.data = NULL;

    /* Read and convert input line. */
    memset( &params, 0, sizeof(TaylorCWParamStruc) );
    I8ToLIGOTimeGPS( &(params.epoch), tStart );
    params.position.system = COORDINATESYSTEM_EQUATORIAL;
    if ( sourcefile ) {
      REAL4Vector *input = NULL; /* input parameters */
      ok &= ( fscanf( fp, "%lli", &epoch ) == 1 );
      SUB( LALSReadVector( &stat, &input, fp ), &stat );
      ok &= ( input->length > 6 );
      if ( ok ) {
	params.aPlus = input->data[0];
	params.aCross = input->data[1];
	params.psi = input->data[2];
	params.position.longitude = input->data[3];
	params.position.latitude = input->data[4];
	params.phi0 = input->data[5];
	params.f0 = input->data[6];
	if ( input->length > 7 ) {
	  SUB( LALSCreateVector( &stat, &(params.f),
				 input->length - 7 ), &stat );
	  for ( i = 0; i < input->length - 7; i++ )
	    params.f->data[i] = input->data[i+7];
	}
      }
    } else {
      params.aPlus = APLUS;
      params.aCross = ACROSS;
      params.psi = PSI;
      params.position.longitude = RA;
      params.position.latitude = DEC;
      params.phi0 = PHI0;
      params.f0 = F0;
    }

    /* Adjust frequency and spindown terms to the actual wave start
       time. */
    if ( params.f ) {
      UINT4 length = params.f->length; /* number of coefficients */
      REAL4 t = (1.0e-9)*(REAL4)( tStart - epoch ); /* time shift */
      REAL4 tN = 1.0;   /* t raised to various powers */
      REAL4 fFac = 1.0; /* fractional change in frequency */
      REAL4 *fData = params.f->data; /* pointer to coeficients */
      for ( i = 0; i < length; i++ ) {
	REAL4 tM = 1.0; /* t raised to various powers */
	fFac += fData[i]*( tN *= t );
	for ( j = i + 1; j < length; j++ )
	  fData[i] += choose( j + 1, i + 1 )*fData[j]*( tM *= t );
      }
      params.f0 *= fFac;
      for ( i = 0; i < length; i++ )
	fData[i] /= fFac;
    }

    if ( ok ) {
      REAL4 *sigData, *outData; /* pointers to time series data */
      REAL4 t = (1.0e-9)*(REAL4)( tStop - tStart ); /* duration */
      REAL4 dtInv = 0.0; /* sampling rate for waveform */

      /* Set remaining parameters, and generate waveform. */
      if ( params.f ) {
	REAL4 tN = 1.0; /* t raised to various powers */
	for ( i = 0; i < params.f->length; i++ ) {
	  dtInv += fabs( params.f0*params.f->data[i] )*tN;
	  tN *= t;
	}
      }
      if ( dtInv < 1.0/t ) {
	params.deltaT = t;
	params.length = 2;
      } else {
	params.deltaT = 1.0/dtInv;
	params.length = (UINT4)( t*dtInv ) + 2;
      }
      SUB( LALGenerateTaylorCW( &stat, &waveform, &params ), &stat );
      if ( params.dfdt > 2.0 ) {
        LALSnprintf( message, MSGLEN,
                     "Waveform sampling interval is too large:\n"
                     "\tmaximum df*dt = %f", params.dfdt );
        WARNING( message );
      }
      SUB( LALSCreateVector( &stat, &(signal.data), npt ), &stat );
      SUB( LALSimulateCoherentGW( &stat, &signal, &waveform,
				  &detector ), &stat );

      /* Inject waveform into output. */
      sigData = signal.data->data;
      outData = output.data->data;
      for ( i = 0; i < (UINT4)( npt ); i++ )
	outData[i] += sigData[i];

      /* Clean up memory. */
      SUB( LALSDestroyVectorSequence( &stat, &( waveform.a->data ) ),
           &stat );
      SUB( LALSDestroyVector( &stat, &( waveform.f->data ) ),
	   &stat );
      SUB( LALDDestroyVector( &stat, &( waveform.phi->data ) ),
	   &stat );
      LALFree( waveform.a );
      LALFree( waveform.f );
      LALFree( waveform.phi );
      SUB( LALSDestroyVector( &stat, &(signal.data) ), &stat );
    }

    /* Inject only one signal if there is no sourcefile. */
    if ( !sourcefile )
      ok = 0;
  }

  /* Input file is exhausted (or has a badly-formatted line ). */
  if ( sourcefile )
    fclose( fp );


  /*******************************************************************
   * CLEANUP                                                         *
   *******************************************************************/

  /* Print output file. */
  if ( outfile ) {
    if ( ( fp = fopen( outfile, "w" ) ) == NULL ) {
      ERROR( SIMULATETAYLORCWTESTC_EFILE,
	     SIMULATETAYLORCWTESTC_MSGEFILE, outfile );
      return SIMULATETAYLORCWTESTC_EFILE;
    }
    epoch = 1000000000LL*(INT8)( output.epoch.gpsSeconds );
    epoch += (INT8)( output.epoch.gpsNanoSeconds );
    fprintf( fp, "# epoch = %lli\n", epoch );
    fprintf( fp, "# deltaT = %23.16e\n", output.deltaT );
    for ( i = 0; i < output.data->length; i++ )
      fprintf( fp, "%8.1f\n", (REAL4)( output.data->data[i] ) );
    fclose( fp );
  }

  /* Destroy remaining memory. */
  SUB( LALSDestroyVector( &stat, &( output.data ) ), &stat );
  SUB( LALCDestroyVector( &stat, &( detector.transfer->data ) ),
       &stat );
  LALFree( detector.transfer );
  if ( site ) {
    LALFree( detector.ephemerides );
    LALFree( detector.site );
  }

  /* Done! */
  LALCheckMemoryLeaks();
  INFO( SIMULATETAYLORCWTESTC_MSGENORM );
  return SIMULATETAYLORCWTESTC_ENORM;
}


/* A function to convert INT8 nanoseconds to LIGOTimeGPS. */
void
I8ToLIGOTimeGPS( LIGOTimeGPS *output, INT8 input )
{
  INT8 s = input / 1000000000LL;
  output->gpsSeconds = (INT4)( s );
  output->gpsNanoSeconds = (INT4)( input - 1000000000LL*s );
  return;
}


/* A function to compute the combination function C(a,b). */
UINT4
choose( UINT4 a, UINT4 b )
{
  UINT4 numer = 1;
  UINT4 denom = 1;
  UINT4 index = b + 1;
  while ( --index ) {
    numer *= a - b + index;
    denom *= index;
  }
  return numer/denom;
}
