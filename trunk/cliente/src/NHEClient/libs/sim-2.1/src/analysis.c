////////////////////////////////////////////////////////////////////////
//								      //
// analysis.c							      //
//							              //
// contains methods for analyzing a histogram. Methods for taking     //
// confidence intervals, for taking a chi square test, for analyzing  //
// simulation behavior and for fitting curves and probability         //
// distributions are included.                                        //
//								      //
// (c) 1995 D. Bolier, A. Eliens   				      //
//								      //
////////////////////////////////////////////////////////////////////////

#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <fstream.h>
#include <sim/defs.h>
#include <sim/error.h>
#include <sim/histogram.h>
#include <sim/analysis.h>
#include <sim/coutanalysis.h>
#include <sim/reports.h>
#include <sim/simulation.h>
#include <sim/converter.h>

#ifdef HUSH

enum {SCREENWIDTH = 400,TEXTHEIGHT = 20};

#endif

// constants
enum {MAXOUTPUT = 50,FILEITEMS = 1000000};

// global functions and variables

static double lngamma(double xx);
static const double cof[6]   = {76.18009173, -86.50532033, 24.01409822, -1.231739516,0.120858003e-2, -0.536382e-5};


// values of the student distribution for a = 0.95, 0.975 and
// 0.995 (corresponding to confidence level of 0.90, 0.95 and 0.99)
// and degree of freedom of 1,2,..,29,30,40,60,120,>120

static double st[3][34] =
  { {6.314, 2.920, 2.353, 2.132, 2.015, 1.943, 1.895, 1.860, 1.833, 1.812, 
      1.796, 1.782, 1.771, 1.761, 1.753, 1.746, 1.740, 1.734, 1.729, 1.725,
      1.721, 1.717, 1.714, 1.711, 1.708, 1.706, 1.703, 1.701, 1.699, 1.697,
      1.684, 1.671, 1.658, 1.645
    },
    {12.706, 4.303, 3.182, 2.776, 2.571,  2.447, 2.365, 2.306, 2.262, 2.228,
      2.201, 2.179, 2.160, 2.145, 2.131,  2.120, 2.110, 2.101, 2.093, 2.086,
      2.080, 2.074, 2.069, 2.064, 2.060,  2.056, 2.052, 2.048, 2.045, 2.042,
      2.021, 2.000, 1.980, 1.960
    },
    { 63.66, 9.925, 5.841, 4.604, 4.032,  3.707, 3.499, 3.355, 3.250, 3.169,
      3.106, 3.055, 3.012, 2.977, 2.947,  2.921, 2.898, 2.878, 2.861, 2.845,
      2.831, 2.819, 2.807, 2.797, 2.787,  2.779, 2.771, 2.763, 2.756, 2.750,
      2.704, 2.660, 2.617, 2.576
    }
  };

// values of the chi square distribution for a = 0.005, 0.01, 0.025,
// 0.05, 0.10, 0.2,..., 0.9,0.95,0.975,0.99,0.995,0.999 and
// degree of freedom 1,2,...,29,30,35,40,45,50,75,100

static double ch[36][18] = 
  { {0.0000393, 0.000157, 0.000982, 0.00393, 0.0158, 0.0642, 0.148, 0.275, 0.455, 0.708, 1.07, 1.64, 2.71, 3.84, 5.02, 6.63, 7.88, 10.8},
    {0.0100, 0.0201, 0.0506, 0.103, 0.211, 0.446, 0.713, 1.02, 1.39, 1.83, 2.41, 3.22, 4.61, 5.99, 7.38, 9.21, 10.6, 13.8},
    {0.0717, 0.115, 0.216, 0.352, 0.584, 1.00, 1.42, 1.87, 2.37, 2.95, 3.67, 4.64, 6.25, 7.81, 9.35, 11.3, 12.8, 16.3},
    {0.207, 0.297, 0.484, 0.711, 1.06, 1.65, 2.19, 2.75, 3.36, 4.04, 4.88, 5.99, 7.78, 9.49, 11.1, 13.3, 14.9, 18.5},
    {0.412, 0.554, 0.831, 1.15, 1.61, 2.34, 3.00, 3.66, 4.35, 5.13, 6.06, 7.29, 9.24, 11.1, 12.8, 15.1, 16.7, 20.5},
    {0.676, 0.872, 1.24, 1.64, 2.20, 3.07, 3.83, 4.57, 5.35, 6.21, 7.23, 8.56, 10.6, 12.6, 14.4, 16.8, 18.5, 22.5},
    {0.989, 1.24, 1.69, 2.17, 2.83, 3.82, 4.67, 5.49, 6.35, 7.28, 8.38, 9.80, 12.0, 14.1, 16.0, 18.5, 20.3, 24.3},
    {1.34, 1.65, 2.18, 2.73, 3.49, 4.59, 5.53, 6.42, 7.34, 8.35, 9.52, 11.0, 13.4, 15.5, 17.5, 20.1, 22.0, 26.1},
    {1.73, 2.09, 2.70, 3.33, 4.17, 5.38, 6.39, 7.36, 8.34, 9.41, 10.7, 12.2, 14.7, 16.9, 19.0, 21.7, 23.6, 27.9},
    {2.16, 2.56, 3.25, 3.94, 4.87, 6.18, 7.27, 8.30, 9.34, 10.5, 11.8, 13.4, 16.0, 18.3, 20.5, 23.2, 25.2, 29.6},
    {2.60, 3.05, 3.82, 4.57, 5.58, 6.99, 8.15, 9.24, 10.3, 11.5, 12.9, 14.6, 17.3, 19.7, 21.9, 24.7, 26.8, 31.3},
    {3.07, 3.57, 4.40, 5.23, 6.30, 7.81, 9.03, 10.2, 11.3, 12.6, 14.0, 15.8, 18.5, 21.0, 23.3, 26.2, 28.3, 32.9},
    {3.57, 4.11, 5.01, 5.89, 7.04, 8.63, 9.93, 11.1, 12.3, 13.6, 15.1, 17.0, 19.8, 22.4, 24.7, 27.7, 29.8, 34.5},
    {4.07, 4.66, 5.63, 6.57, 7.79, 9.47, 10.8, 12.1, 13.3, 14.7, 16.2, 18.2, 21.1, 23.7, 26.1, 29.1, 31.3, 36.1},
    {4.60, 5.23, 6.26, 7.26, 8.55, 10.3, 11.7, 13.0, 14.3, 15.7, 17.3, 19.3, 22.3, 25.0, 27.5, 30.6, 32.8, 37.7},
    {5.14, 5.81, 6.91, 7.96, 9.31, 11.2, 12.6, 14.0, 15.3, 16.8, 18.4, 20.5, 23.5, 26.3, 28.8, 32.0, 34.3, 39.3},
    {5.70, 6.41, 7.56, 8.67, 10.1, 12.0, 13.5, 14.9, 16.3, 17.8, 19.5, 21.6, 24.8, 27.6, 30.2, 33.4, 35.7, 40.8},
    {6.26, 7.01, 8.23, 9.39, 10.9, 12.9, 14.4, 15.9, 17.3, 18.9, 20.6, 22.8, 26.0, 28.9, 31.5, 34.8, 37.2, 42.3},
    {6.84, 7.63, 8.91, 10.1, 11.7, 13.7, 15.4, 16.9, 18.3, 19.9, 21.7, 23.9, 27.2, 30.1, 32.9, 36.2, 38.6, 43.8},
    {7.43, 8.26, 9.59, 10.9, 12.4, 14.6, 16.3, 17.8, 19.3, 21.0, 22.8, 25.0, 28.4, 31.4, 34.2, 37.6, 40.0, 45.3},
    {8.03, 8.90, 10.3, 11.6, 13.2, 15.4, 17.2, 18.8, 20.3, 22.0, 23.9, 26.9, 29.6, 32.7, 35.5, 38.9, 41.4, 46.8},
    {8.64, 9.54, 11.0, 12.3, 14.0, 16.3, 18.1, 19.7, 21.3, 23.0, 24.9, 27.3, 30.8, 33.9, 36.8, 40.3, 42.8, 48.3},
    {9.26, 10.2, 11.7, 13.1, 14.8, 17.2, 19.0, 20.7, 22.3, 24.1, 26.0, 28.4, 32.0, 35.2, 38.1, 41.6, 44.2, 49.7},
    {9.89, 10.9, 12.4, 13.8, 15.7, 18.1, 19.9, 21.7, 23.3, 25.1, 27.1, 29.6, 33.2, 36.4, 39.4, 43.0, 45.6, 51.2},
    {10.5, 11.5, 13.1, 14.6, 16.5, 18.9, 20.9, 22.6, 24.3, 26.1, 28.2, 30.7, 34.4, 37.7, 40.6, 44.3, 46.9, 52.6},
    {11.2, 12.2, 13.8, 15.4, 17.3, 19.8, 21.8, 23.6, 25.3, 27.2, 29.2, 31.8, 35.6, 38.9, 41.9, 45.6, 48.3, 54.1},
    {11.8, 12.9, 14.6, 16.2, 18.1, 20.7, 22.7, 24.5, 26.3, 28.2, 30.3, 32.9, 36.7, 40.1, 43.2, 47.0, 49.6, 55.5},
    {12.5, 13.6, 15.3, 16.9, 18.9, 21.6, 23.6, 25.5, 27.3, 29.2, 31.4, 34.0, 37.9, 41.3, 44.5, 48.3, 51.0, 56.9},
    {13.1, 14.3, 16.0, 17.7, 19.8, 22.5, 24.6, 26.5, 28.3, 30.3, 32.5, 35.1, 39.1, 42.6, 45.7, 49.6, 52.3, 58.3},
    {13.8, 15.0, 16.8, 18.5, 20.6, 23.4, 25.5, 27.4, 29.3, 31.3, 33.5, 36.3, 40.3, 43.8, 47.0, 50.9, 53.7, 59.7},
    {17.2, 18.5, 20.6, 22.5, 24.8, 27.8, 30.2, 32.3, 34.3, 36.5, 38.9, 41.8, 46.1, 49.8, 53.2, 57.3, 60.3, 66.6},
    {20.7, 22.2, 24.4, 26.5, 29.1, 32.3, 34.9, 37.1, 39.3, 41.6, 44.2, 47.3, 51.8, 55.8, 59.3, 63.7, 66.8, 73.4},
    {24.3, 25.9, 28.4, 30.6, 33.4, 36.9, 39.6, 42.0, 44.3, 46.8, 49.5, 52.7, 57.5, 61.7, 65.4, 70.0, 73.2, 80.1},
    {28.0, 29.7, 32.4, 34.8, 37.7, 41.4, 44.3, 46.9, 49.3, 51.9, 54.7, 58.2, 63.2, 67.5, 71.4, 76.2, 79.5, 86.7},
    {47.2, 49.5, 52.9, 56.1, 59.8, 64.5, 68.1, 71.3, 74.3, 77.5, 80.9, 85.1, 91.1, 96.2, 100.8, 106.4, 110.3, 118.6},
    {67.3, 70.1, 74.2, 77.9, 84.2, 87.9, 92.1, 95.8, 99.3, 102.9, 106.9, 111.7, 118.5, 124.3, 129.6, 135.6, 140.2, 149.4}
  };


// constructor, sets histogram to be analyzed and creates output 
// structure

analysis::analysis(histogram* hi)
{
  h = hi;
  ou = new coutanalysis(hi,this);
#ifdef HUSH
  textoption = new char[BUFSIZ];
  sprintf(textoption,"-fill black");
  fit = new char[BUFSIZ];
  sprintf(fit,"-fill red");
#endif
}


// destructor, dereferences the histogram pointer and deallocates the
// memory for the output structure

analysis::~analysis()
{
  h = NULL;
  delete ou;
#ifdef HUSH
  delete textoption;
  delete fit;
#endif
}


// give confidence interval with level cl, according to the normal
// distribution and assuming a two-tailed test

double analysis::confidencenormal(level cl)
{
  double n = h -> sumfrequencies();
  if (cl == PERCENT90)
    return ( 1.64 * h -> standarddeviation() / sqrt(n) );
  else if (cl == PERCENT95) 
    return ( 1.96 * h -> standarddeviation() / sqrt(n) );
  else
    return ( 2.58 * h -> standarddeviation() / sqrt(n) );
}


// give confidence interval with level cl for the difference of the
// means of the histogram to be analyzed and the given histogram, 
// according to the normal distribution and assuming a two-tailed test
// The two histograms have equal variances

double analysis::confidencenormal(level cl,histogram* hi)
{
  double var1 = h -> variance();
  double n1 = h -> sumfrequencies();
  double n2 = hi -> sumfrequencies();
  double stddev = sqrt(var1);
  if (cl == PERCENT90)
    return (1.64 * stddev * sqrt(1/n1+1/n2));
  else if (cl == PERCENT95) 
    return (1.96 * stddev * sqrt(1/n1+1/n2));
  else
    return (2.58 * stddev * sqrt(1/n1+1/n2) );
}


// give confidence interval with level cl, according to the student
// distribution and assuming a two-tailed test

double analysis::confidencestudent(level cl)
{
  int dof = (int)h -> sumfrequencies()-1;
  int p = stindex(dof);
  if (cl == PERCENT90)
    return ( st[0][p] * h -> standarddeviation() / sqrt(1.0+dof));
  else if (cl == PERCENT95) 
    return ( st[1][p] * h -> standarddeviation() / sqrt(1.0+dof));
  else
    return ( st[2][p] * h -> standarddeviation() / sqrt(1.0+dof));
}


// give confidence interval with level cl for the difference of the
// means of the histogram to be analyzed and the given histogram, 
// according to the student distribution and assuming a two-tailed test

double analysis::confidencestudent(level cl,histogram* hi)
{
  int dof = (int)(h -> sumfrequencies() + hi -> sumfrequencies()-2);
  int p = stindex(dof);
  double var1 = h -> variance();
  double var2 = hi -> variance();
  double n1 = h -> sumfrequencies();
  double n2 = hi -> sumfrequencies();
  double stddev = sqrt(((n1-1)*var1+(n2-1)*var2) / (n1+n2-2));
  if (cl == PERCENT90)
    return ( st[0][p] * stddev * sqrt(1/n1+1/n2));
  else if (cl == PERCENT95) 
    return ( st[1][p] * stddev * sqrt(1/n1+1/n2));
  else
    return ( st[2][p] * stddev * sqrt(1/n1+1/n2));
}


// return index in the student array, given the degree of freedom

int analysis::stindex(int dof)
{
  if (dof > 120)
    return 33;
  if (dof > 60)
    return 32;
  if (dof > 40)
    return 31;
  if (dof > 30)
    return 30;
  return dof-1;
} 


// return index in the chi square array, given the degree of freedom

int analysis::chindex(int dof)
{
  if (dof > 100)
    return 35;
  if (dof > 75)
    return 34;
  if (dof > 50)
    return 33;
  if (dof > 45)
    return 32;
  if (dof > 40)
    return 31;
  if (dof > 35)
    return 30;
  if (dof > 30)
    return 29;
  if (dof < 1)
    return 0;
  return dof-1;
} 


// return chi square probability given the index in the chi square array
// the index is an integer between 0 and 17

double analysis::prob(int index)
{
  if (index == 0)
    return 0.005;
  if (index == 1)
    return 0.010;
  if (index == 2)
    return 0.025;
  if (index == 3)
    return 0.05;
  if (index <= 12)
    return (0.10 + (index-4) * 0.10);
  if (index == 13)
    return 0.95;
  if (index == 14)
    return 0.975;
  if (index == 15)
    return 0.990;
  if (index == 16)
    return 0.995;
  if (index == 17)
    return 0.999;
}


// return chi square probability, given the chi square statistic and
// the index in the array

double analysis::chisquareprobability(double chisq,int index)
{
  // search for the right value
  int i=0;
  while  ( (ch[index][i] < chisq) && (i < 18) )
    i++;
  if (i == 0)
    return 1.0;
  else if (i == 18)
    return 0.0;

  // determine factor between statistic and border value
  double factor = (chisq - ch[index][i-1]) / (ch[index][i] - ch[index][i-1]);

  // return probability (the chance that the statistic was greater, 
  // so 1-..)
  return (1 - (prob(i-1) + factor * (prob(i) - prob(i-1))));
}


// return chi square probability of the array expected and the values
// of the histogram. Format output structure

double analysis::chisquaretest(double* expected)
{
  double* observed = h -> values();
  int nc = h -> columns();

  // degree of freedom
  int dof = nc-1;

  // chi square statistic is the sum of the square root of the 
  // deviations
  double chisq = 0.0;
  int i = 0;
  while (i < nc)
  {
    int j = i+1;

    // if the expected value is less then 5, add it to the last column,
    // that has a value greater then 5
    double sumobserved = 0.0;
    double sumexpected = 0.0;
    while ( (j < nc) && (expected[j] < 5) )
    {
      // adjust 
      sumobserved += observed[j+1]; 
      sumexpected += expected[j];
      j++;
      dof--;
    }
    double deviation;
    if ( (expected[i]+sumexpected) == 0.0 )
      deviation = (observed[i+1]+sumobserved) * (observed[i+1]+sumobserved);
    else
      deviation = (observed[i+1]+sumobserved - expected[i]-sumexpected) * (observed[i+1]+sumobserved - expected[i]-sumexpected) / fabs(expected[i]+sumexpected);
    chisq += deviation;

    // if no adjustments simply i++, else i = j
    i = j;
  }

  int index = chindex(dof);  

  // get chi square probability
  double p = chisquareprobability(chisq,index);

  ou -> chisquaretest(dof,chisq,p);
  cout << (*ou);
  return p;
}
   

// same as the one before, except the formatting
// it also sets the statistic and the degree of freedom

double analysis::chisquaretest(double* expected,int& dof,double& chisq)
{
  double* observed = h -> values();
  int nc = h -> columns();

  // degree of freedom
  dof = nc-1;

  // chi square statistic is the sum of the square root of the 
  // deviations
  int i = 0;
  while (i < nc)
  {
    int j = i+1;

    // if the expected value is less then 5, add it to the last column,
    // that has a value greater then 5
    double sumobserved = 0.0;
    double sumexpected = 0.0;
    while ( (j < nc) && (expected[j] < 5) )
    {
      // adjust
      sumobserved += observed[j+1];
      sumexpected += expected[j];
      j++;
      dof--;
    }
    double deviation;
    if ( (expected[i]+sumexpected) == 0.0 )
      deviation = (observed[i+1]+sumobserved) * (observed[i+1]+sumobserved);
    else
      deviation = (observed[i+1]+sumobserved - expected[i]-sumexpected) * (observed[i+1]+sumobserved - expected[i]-sumexpected) / fabs(expected[i]+sumexpected);
    chisq += deviation;
    // if no adjustments simply i++, else i = j
    i = j;
  }

  int index = chindex(dof);  

  // get chi square probability
  double p = chisquareprobability(chisq,index);
  return p;
}
   

// return confidence interval of the histogram to be analyzed, given 
// the level. 

double analysis::confidence(char* options)
{
  level cl = PERCENT95;
  converter* cnv = new converter(options);
  if ((cnv -> argc() == 2) && (!strcmp((*cnv)[1],"-percent")))
    if (!strcmp((*cnv)[2],"90"))
      cl = PERCENT90; 
    else if (!strcmp((*cnv)[2],"95"))
      cl = PERCENT95; 
    else if (!strcmp((*cnv)[2],"99"))
      cl = PERCENT99; 
    else
      err(LEVEL);

  // take normal distribution if the sum of frequencies is greater then
  // 30, else take the student distribution
  double conf;
  if (h -> sumfrequencies() > 30.0)
    conf = confidencenormal(cl);
  else
    conf = confidencestudent(cl);

  ou -> confidence(cl,conf,h -> mean());
  cout << (*ou);
  delete cnv;
  return conf;
}


// return confidence interval for the difference of means, given the
// the level.

double analysis::confidence(histogram* hi,char* options)
{
  level cl = PERCENT95;
  converter* cnv = new converter(options);
  if ((cnv -> argc() == 2) && (!strcmp((*cnv)[1],"-percent")))
    if (!strcmp((*cnv)[2],"90"))
      cl = PERCENT90; 
    else if (!strcmp((*cnv)[2],"95"))
      cl = PERCENT95; 
    else if (!strcmp((*cnv)[2],"99"))
      cl = PERCENT99; 
    else
      err(LEVEL);

  // estimated mean
  double estimated = fabs(h -> mean() - hi -> mean());

  // take normal distribution if equal variances and sum of frequencies
  // greater then 30
  double conf;
  if ( (h -> variance() != hi -> variance()) && (h -> sumfrequencies() > 30.0) )
    conf = confidencenormal(cl,hi);
  else
    conf = confidencestudent(cl,hi);

  ou -> difference(cl,conf,estimated);
  cout << (*ou);
  delete cnv;
  return conf;
}


// just return the covariance

double analysis::covar(histogram* h2)
{
  char* fn1 = h -> name();
  char* fn2 = h2 -> name();
  if ( (!fn1) || (!fn2) )
    err(SAMPLE);
  h -> closefile();
  h2 -> closefile();
  ifstream* fp1 = new ifstream(fn1);
  ifstream* fp2 = new ifstream(fn2);
  char* buf = new char[BUFSIZ];
  double* val1 = new double[FILEITEMS];
  double* val2 = new double[FILEITEMS];
  double* freq1 = new double[FILEITEMS];
  double* freq2 = new double[FILEITEMS];
  double mn1 = h -> mean();
  double mn2 = h2 -> mean();

  int i1 = 0;
  while ((*fp1) >> buf) 
  {
    val1[i1] = atof(buf);
    (*fp1) >> buf; 
    freq1[i1] = atof(buf);
    i1++;
  }
  int i2 = 0;
  while ((*fp2) >> buf)
  {
    val2[i2] = atof(buf);
    (*fp2) >> buf; 
    freq2[i2] = atof(buf);
    i2++;
  }

  int j = 0;int k = 0;

  double sum = 0;double n = 0;
  // covariance, use equal frequencies !
  while ( (j < i1) && (k < i2) )
  {
    // first frequency less
    if (freq1[j] < freq2[k])
    {
      sum += (val1[j]-mn1) * (val2[k]-mn2) * freq1[j];
      n += freq1[j];
      freq2[k] -= freq1[j];
      j++;
    }
    // second frequency less
    else if (freq2[k] < freq1[j])
    {
      sum += (val1[j]-mn1) * (val2[k]-mn2) * freq2[k];
      n += freq2[k];
      freq1[j] -= freq2[k];
      k++;
    }
    // equal frequencies
    else 
    {
      sum += (val1[j]-mn1) * (val2[k]-mn2) * freq1[j];
      n += freq1[j];
      k++;  
      j++;
    }
  }
  double cov = sum/(n-1);

  delete val1;
  delete val2;
  delete freq1;
  delete freq2;
  delete buf;

  return cov;
}


// covariance of two histograms (with raw samples in the two files)

double analysis::covariance(histogram* h2)
{
  double cov = covar(h2);
  char* fn1 = h -> name();
  char* fn2 = h2 -> name();

  ou -> covariance(fn1,fn2,cov);
  cout << (*ou);
  return cov;
}
  

// correlation of two histograms (with raw samples in the two files)

double analysis::correlation(histogram* h2)
{
  if ( (!(h -> variance())) || (!(h2 -> variance())) )
    return 0;
  double cor = covar(h2) / sqrt(h -> variance()*h2 -> variance());
  char* fn1 = h -> name();
  char* fn2 = h2 -> name();

  ou -> correlation(fn1,fn2,cor);
  cout << (*ou);
  return cor;
}
  

// fit a linear curve, according to the method of the least squares. 
// Return the belonging chi square probability

double analysis::linear()
{
  int nc = h -> columns();
  double wd = h -> widthcolumn();
  double strt = h -> startvalue();
  double* dt = h -> values();

  // determine sum of the x, x*x, y and x*y values
  // notice that we take the half of a column as the x value
  double sumxx,sumx,sumxy,sumy;
  sumx = sumy = sumxy = sumxx = 0.0;
  for (int i=0;i<nc;i++)
  {
    sumxy += (strt+(i*wd+wd*0.5)) * dt[i+1];
    sumxx += (strt+(i*wd+wd*0.5)) * (strt+(i*wd+wd*0.5));
    sumx += (strt+(i*wd+wd*0.5));
    sumy += dt[i+1];
  }

  // no determinant
  if ( (sumxx == sumx) || (sumx*sumx == nc*sumxx) )
  {
    warn(NOFIT);
    return 0;
  }

  // mimimize sum(y - (ax+b))^2, so a*sumx + n*b = sumy and
  // a*sumxx + b*sumx = sumxy and we have :
  double a = (sumxy-sumx*sumy/nc) / (sumxx-(sumx*sumx/nc));
  double b = (sumy-a*sumx)/nc;

  return linear(a,b);
}


// fit a linear curve, with the given parameters
// Return the belonging chi square probability

double analysis::linear(double a,double b)
{
  int nc = h -> columns();
  double wd = h -> widthcolumn();
  double strt = h -> startvalue();

  // fill array with the linear values
  double* expected = new double[nc];
  for (int i=0;i<nc;i++)
    expected[i] = a * (strt+(i*wd+wd*0.5)) + b;

  // perform chi square test
  int dof; double chisq;
  double p = chisquaretest(expected,dof,chisq);
  delete expected;

  ou -> linear(a,b,dof,chisq,p);
  cout << (*ou);
  return p;
}


// fit quadratic curve according to the least squares method.
// Return chi square probability.

double analysis::quadratic()
{
  int nc = h -> columns();
  double wd = h -> widthcolumn();
  double strt = h -> startvalue();
  double* dt = h -> values();
  double sumx4,sumx3,sumx2,sumx,sumxy,sumx2y,sumy;
  sumx4 = sumx3 = sumx2 = sumx = sumxy = sumx2y = sumy = 0.0;

  // determine sums
  for (int i=0;i<nc;i++)
  {
    sumx4 += (strt+(i*wd+wd*0.5)) * (strt+(i*wd+wd*0.5)) * (strt+(i*wd+wd*0.5)) * (strt+(i*wd+wd*0.5));
    sumx3 += (strt+(i*wd+wd*0.5)) * (strt+(i*wd+wd*0.5)) * (strt+(i*wd+wd*0.5));
    sumx2 += (strt+(i*wd+wd*0.5)) * (strt+(i*wd+wd*0.5));
    sumx += (strt+(i*wd+wd*0.5));
    sumxy += (strt+(i*wd+wd*0.5)) * dt[i+1];
    sumx2y += (strt+(i*wd+wd*0.5)) * (strt+(i*wd+wd*0.5)) *dt[i+1];
    sumy += dt[i+1];
  }

  // check if the determinant exists
  if ( (sumx2*sumx2*sumx2-sumx2*sumx*sumx3-sumx*sumx3*sumx2+nc*sumx3*sumx3+sumx*sumx*sumx4-nc*sumx2*sumx4) == 0.0 )
  {
    warn(NOFIT);
    return 0;
  }

  // minimize sum(y - (axx+bx+c)) so a*sumxx + b*sumx + n*c = sumy,
  // a*sumxxx + b*sumxx + c*sumx = sumxy and a*sumxxxx + b*sumxxx +
  // c*sumxx = sumxxy and we have :
  double c1 = (sumx2-sumx4/sumx2*nc)-(sumx3-sumx4/sumx2*sumx)/(sumx2-sumx3/sumx2*sumx)*(sumx-sumx3/sumx2*nc);
  double c2 = (sumx2y-sumx4/sumx2*sumy)-(sumx3-sumx4/sumx2*sumx)/(sumx2-sumx3/sumx2*sumx)*(sumxy-sumx3/sumx2*sumy);
  double c = c2/c1;
  double b = ((sumxy-sumx3/sumx2*sumy)-(sumx-sumx3/sumx2*nc)*c)/(sumx2-sumx3/sumx2*sumx);
  double a = (sumy-b*sumx-c*nc)/sumx2;

  return quadratic(a,b,c);
}
  

// fit quadratic curve with the given parameters
// Return chi square probability.

double analysis::quadratic(double a,double b,double c)
{
  int nc = h -> columns();
  double wd = h -> widthcolumn();
  double strt = h -> startvalue();

  // fill expected with quadratic values and determine chi square
  // probability
  double* expected = new double[nc];
  for (int i=0;i<nc;i++)
    expected[i] = a*(strt+(i*wd+wd*0.5))*(strt+(i*wd+wd*0.5)) + b*(strt+(i*wd+wd*0.5)) + c;
  int dof;double chisq;
  double p = chisquaretest(expected,dof,chisq);
  delete expected;

  ou -> quadratic(a,b,c,dof,chisq,p);
  cout << (*ou);
  return p;
}
  

// fit the uniform distribution, return chi square probability

double analysis::uniform()
{
  // parameter of the uniform distribution is the maximal minus the
  // minimal value of the histogram
  double a = h -> minimum();
  double b = h -> maximum(); 

  return uniform(a,b);
}


// fit the uniform distribution with the given parameters, return chi 
// square probability

double analysis::uniform(double a,double b)
{
  int nc = h -> columns();
  double sumf = h -> sumfrequencies();
  double wd = h -> widthcolumn();
  double st = h -> startvalue();

  // fill an array with the uniform values and perform the chi square
  // test
  double* expected = new double[nc];
  for (int i=0;i<nc;i++)
  {
    if ( ((st + (i+0.5)*wd) >= a) && ((st + (i+0.5)*wd) <= b) )
      expected[i] = sumf / (b-a) * h -> widthcolumn();
    else
      expected[i] = 0;
  }
  int dof;double chisq;
  double p = chisquaretest(expected,dof,chisq);
  delete expected;

  ou -> uniform(a,b,dof,chisq,p);
  cout << (*ou);
  return p;
}


// fit exponential distribution, return chi square probability

double analysis::exponential() 
{
  // parameter is the mean of the histogram
  double a = h -> mean();

  return exponential(a);
}


// fit exponential distribution with the given parameter, return chi 
// square probability 

double analysis::exponential(double a) 
{
  int nc = h -> columns();

  // fill expected and perform the chi square test
  double* expected = new double[nc];
  for (int i=0;i<nc;i++)
  {
    // pdf : 1/a * exp(-j/a)
    double j = h -> startvalue() + (i + 0.5) * h -> widthcolumn();
    if (j >= 0)
    {
      double probj = 1.0 / a * exp(-j / a);
      expected[i] = h -> sumfrequencies() * probj * h -> widthcolumn();
    } 
    else
      expected[i] = 0;
  }
  int dof;double chisq;
  double p = chisquaretest(expected,dof,chisq);
//  delete expected;  // NO      <----------
  delete[] expected;  // YES     <----------

  ou -> exponential(a,dof,chisq,p);
  cout << (*ou);
  return p;
}


// fit hyperexponential distribution, return chi square probability
// P is known.

double analysis::hyperexponential(double p) 
{
  double mn = h -> mean();

  // mean = pa + (1-p)b en
  // variance = p(2-p)aa - 2p(1-p)ab + (1-pp)bb

  double c = h -> variance() / mn / mn;
  double x = (2*p-p*p)*mn*mn/p/p - h -> variance();
  double y = -2*mn*(2*p-p*p)*(1-p)/p/p - 2*(1-p)*mn;
  double z = (2*p-p*p)*(1-p)*(1-p)/p/p + 2*(1-p)*(1-p) + (1-p*p);

  double d;
  if ((d = y*y-4*x*z) > 0)
  {
    // two possibilities
    double b1 = (-y - sqrt(d)) / (2*z);
    double a1 = (mn-(1-p)*b1) / p;
    double b2 = (-y + sqrt(d)) / (2*z);
    double a2 = (mn-(1-p)*b2) / p;
    hyperexponential(p,a1,b1);
    return (hyperexponential(p,a2,b2));
  }
  else if ((d = y*y-4*x*z) == 0)
  {
    double b1 = (-y - sqrt(d)) / (2*z);
    double a1 = (mn-(1-p)*b1) / p;
    return (hyperexponential(p,a1,b1));
  }
  else
  {
    warn(HYPEREXP);
    return 0;
  }
}


// fit hyperexponential distribution with the given parameters, return 
// chi square probability

double analysis::hyperexponential(double p,double a,double b) 
{
  int nc = h -> columns();

  if ( (p <= 0) || (p >= 1) )
    err(PROBVAL);

  // fill expected and perform the chi square test
  double* expected = new double[nc];
  for (int i=0;i<nc;i++)
  {
    // pdf : 1/a * exp(-j/a)
    double j = h -> startvalue() + (i + 0.5) * h -> widthcolumn();
    if (j >= 0)
    {
      double probj = p / a * exp(-j / a) + (1-p) / b * exp(-j / b);
      expected[i] = h -> sumfrequencies() * probj * h -> widthcolumn();
    } 
    else
      expected[i] = 0;
  }
  int dof;double chisq;
  double pchi = chisquaretest(expected,dof,chisq);
  delete expected;

  ou -> hyperexponential(p,a,b,dof,chisq,pchi);
  cout << (*ou);
  return pchi;
}


// fit the gamma distribution, return chi square probability

double analysis::gamma() 
{
  // determine the parameters for the gamma distribution
  // mean = a/b and variance = a/(b*b), so :
  double b = h -> mean() / h -> variance();
  double a = b * h -> mean();

  return gamma(a,b);
}


// fit the gamma distribution with the given parameters, return chi 
// square probability

double analysis::gamma(double a,double b) 
{
  // fill expected and get chi square probability
  int nc = h -> columns();
  double* expected = new double[nc];
  for (int i=0;i<nc;i++)
  {
    // pdf = b^a * j^(a-1) * exp(-j*b) / gammafunction(a)
    double j = h -> startvalue() + (i + 0.5) * h -> widthcolumn();
    if (j >= 0)
    {
      double probj = pow(b,a) * pow(j,a-1) * exp(-j*b) / exp(lngamma(a));
      expected[i] = h -> sumfrequencies() * probj * h -> widthcolumn();
    }
    else
      expected[i] = 0;
  }
  int dof;double chisq;
  double p = chisquaretest(expected,dof,chisq);
  delete expected;

  ou -> gamma(a,b,dof,chisq,p);
  cout << (*ou);
  return p;
}


// fit erlang distribution, return chi square probability

double analysis::erlang() 
{
  // determine parameters for the erlang distribution,
  // mean = n/a and variance = n/(a^2), where n is a positive 
  // integer, so we have :
  double a = h -> mean() / h -> variance();
  double nf = a * h -> mean();
  unsigned int n;
  if ( (nf - (int)nf) > 0.5 )
    n = (unsigned int)ceil(nf);
  else
    n = (unsigned int)floor(nf);

  return erlang(n,a);
}


// fit erlang distribution with the given parametes, return chi square 
// probability

double analysis::erlang(unsigned int n,double a) 
{
  // fill expected and get chi square probability
  int nc = h -> columns();
  double* expected = new double[nc];
  for (int i=0;i<nc;i++)
  {
    // pdf : a * (a*j)^(n-1) * exp(-j * a) / (n-1)!
    double j = h -> startvalue() + (i + 0.5) * h -> widthcolumn();
    if (j >= 0)
    {
      double probj = a * pow(a*j,n-1) * exp(-j*a) / exp(lngamma(n));
      expected[i] = h -> sumfrequencies() * probj * h -> widthcolumn();
    }
    else
      expected[i] = 0;
  }
  int dof;double chisq;
  double p = chisquaretest(expected,dof,chisq);
  delete expected;

  ou -> erlang(a,n,dof,chisq,p);
  cout << (*ou);
  return p;
}


// fit binomial distribution, return chi square probability

double analysis::binomial() 
{
  // determine parameters for the binomial distribution
  // mean = n*p and variance = n*p*(1-p), so
  double p = 1.0 - h -> variance() / h -> mean();
  if ( (p < 0.0) || (p > 1.0) )
    return 0.0;
  double nf = h -> mean() / p;
  unsigned int n;
  if ( (nf - (int)nf) > 0.5 )
    n = (unsigned int)ceil(nf);
  else
    n = (unsigned int)floor(nf);

  return binomial(p,n);
}


// fit binomial distribution with the given parameters, return chi 
// square probability

double analysis::binomial(double p,unsigned int n) 
{
  int nc = h -> columns();

  if ( (p <= 0) || (p >= 1) )
    err(PROBVAL);

  // fill expected and get chi square probability
  double* expected = new double[nc];
  for (int i=0;i<nc;i++)
  {
    // pdf : n!/(j!*(n-j)!) * p^x * (1-p)^(n-x)
    double j = h -> startvalue() + (i + 0.5) * h -> widthcolumn();
    if (j >= 0)
    {
      double coeff = exp(lngamma(n+1)) / exp(lngamma((int)j+1)) / exp(lngamma(n-(int)j+1));
      double probj = coeff * pow(p,(int)j) * pow(1-p,n-(int)j);
      expected[i] = h -> sumfrequencies() * probj * h -> widthcolumn();
    }
    else
      expected[i] = 0;
  }
  int dof;double chisq;
  double pchi = chisquaretest(expected,dof,chisq);
  delete expected;

  ou -> binomial(n,p,dof,chisq,pchi);
  cout << (*ou);
  return pchi;
}


// fit geometric distribution, return chi square probability

double analysis::geometric() 
{
  // determine parameters for the geometric distribution
  // mean = 1/p and variance = (1-p)/p^2, so we have
  double p = 1.0 / h -> mean();

  return geometric(p);
}


// fit geometric distribution with the given parameter, return chi 
// square probability

double analysis::geometric(double p) 
{
  int nc = h -> columns();

  if ( (p <= 0) || (p >= 1) )
    err(PROBVAL);

  // fill expected and determine chi square probability
  double* expected = new double[nc];
  for (int i=0;i<nc;i++)
  {
    // pdf : (1-p)^(x-1) * p
    double j = h -> startvalue() + (i + 0.5) * h -> widthcolumn();
    if (j >= 1)  // j >= 1
    {
      double probj = pow(1-p,(int)j-1) * p;
      expected[i] = h -> sumfrequencies() * probj * h -> widthcolumn();
    }
    else
      expected[i] = 0.0;
  }
  int dof;double chisq;
  double pchi = chisquaretest(expected,dof,chisq);
  delete expected;

  ou -> geometric(p,dof,chisq,pchi);
  cout << (*ou);
  return pchi;
}


// fit hypergeometric distribution, return chi square probability
// n is the number of items drawn which is always known

double analysis::hypergeometric(unsigned int n) 
{
  // mean = np and
  // variance = np(1-p)(m-n)/(m-1) so
  double p = h -> mean() / n;
  unsigned int m = (int)((n*n*p*(1-p) - h -> variance()) / (n*p*(1-p) - h -> variance()));

  return hypergeometric(m,n,p);
}


// fit hypergeometric distribution with the given parameters, return 
// chi square probability

double analysis::hypergeometric(unsigned int m,unsigned int n,double p) 
{
  int nc = h -> columns();

  if ( (p <= 0) || (p >= 1) )
    err(PROBVAL);

  int mp = (int)(m*p);

  // fill expected and determine chi square probability
  double* expected = new double[nc];
  for (int i=0;i<nc;i++)
  {
    // pdf : as below
    int j = (int)(h -> startvalue() + (i + 0.5) * h -> widthcolumn());
    if ( (j >= 0) && (j <= m*p) && (j <= n) )
    {
      // fac(i) = lngamma(i+1) for integers
      double c1 = exp(lngamma(mp+1)) / exp(lngamma(j+1)) / exp(lngamma(mp-j+1));
      double c2 = exp(lngamma(m-mp+1)) / exp(lngamma(n-j+1)) / exp(lngamma(m-mp-n+j+1));
      double c3 = exp(lngamma(m+1)) / exp(lngamma(n+1)) / exp(lngamma(m-n+1));
      double probj = c1 * c2 / c3;
      expected[i] = h -> sumfrequencies() * probj * h -> widthcolumn();
    }
    else
      expected[i] = 0;
  }
  int dof;double chisq;
  double pchi = chisquaretest(expected,dof,chisq);
  delete expected;

  ou -> hypergeometric(m,n,p,dof,chisq,pchi);
  cout << (*ou);
  return pchi;
}


// fit the negative binomial distribution. Return chi square probability

double analysis::negativebinomial() 
{
  // determine parameters for the negative binomial distribution
  // mean = rf * (1-p) / p and variance = rf * (1-p) / p^2, where
  // rf is an integer, so we have :
  double p = h -> mean() / h -> variance();
  double rf = h -> mean() * p / (1-p);
  unsigned int n;
  if ( (rf - (int)rf) > 0.5 )
    n = (unsigned int)ceil(rf);
  else
    n = (unsigned int)floor(rf);

  return negativebinomial(p,n);
}


// fit the negative binomial distribution with the given parameters. 
// Return chi square probability

double analysis::negativebinomial(double p,unsigned int n) 
{
  // fill expected and get chi square probability
  int nc = h -> columns();

  if ( (p <= 0) || (p >= 1) )
    err(PROBVAL);

  double* expected = new double[nc];
  for (int i=0;i<nc;i++)
  {
    // pdf : (n+j-1)!/(j!*(n-1)!) * p^n * (1-p)^j
    double j = h -> startvalue() + (i + 0.5) * h -> widthcolumn();
    if (j >= 0)
    {
      double coeff = exp(lngamma(n+(int)j)) / exp(lngamma((int)j+1)) / exp(lngamma(n));
      double probj = coeff * pow(p,n) * pow(1-p,(int)j);
      expected[i] = h -> sumfrequencies() * probj * h -> widthcolumn();
    }
    else
      expected[i] = 0;
  }
  int dof;double chisq;
  double pchi = chisquaretest(expected,dof,chisq);
  delete expected;

  ou -> negativebinomial(p,n,dof,chisq,pchi);
  cout << (*ou);
  return pchi;
}


// fit poisson distribution, return chi square probability

double analysis::poisson() 
{
  // determine parameter for the poisson distribution
  double a = h -> mean();

  return poisson(a);
}


// fit poisson distribution with the given parameter, return chi square 
// probability

double analysis::poisson(double a) 
{
  int nc = h -> columns();

  // fill expected and get chi square probability
  double* expected = new double[nc];
  for (int i=0;i<nc;i++)
  {
    // pdf : a^j * exp(-a)/j!
    double j = h -> startvalue() + (i + 0.5) * h -> widthcolumn();
    if (j >= 0)
    {
      double probj = pow(a,(int)j) * exp(-a) / exp(lngamma((int)j+1));
      expected[i] = h -> sumfrequencies() * probj * h -> widthcolumn();
    }
    else
      expected[i] = 0;
  }
  int dof;double chisq;
  double p = chisquaretest(expected,dof,chisq);
  delete expected;

  ou -> poisson(a,dof,chisq,p);
  cout << (*ou);
  return p;
}


// fit normal distribution, return chi square probability

double analysis::normal()
{
  // determine parameters
  double a = h -> mean();
  double b = h -> standarddeviation();

  return normal(a,b);
}


// fit normal distribution with the given parameters, return chi square 
// probability

double analysis::normal(double a,double b)
{
  int nc = h -> columns();

  // fill expected and get chi square probability
  double* expected = new double[nc];
  for (int i=0;i<nc;i++)
  {
    // pdf : as below
    double j = h -> startvalue() + (i + 0.5) * h -> widthcolumn();
    double probj = exp(-(j-a)*(j-a)/2/b/b) / sqrt(2*PI) / b;
    expected[i] = h -> sumfrequencies() * probj * h -> widthcolumn();
  }
  int dof;double chisq;
  double p = chisquaretest(expected,dof,chisq);
  delete expected;

  ou -> normal(a,b,dof,chisq,p);
  cout << (*ou);
  return p;
}


// fit beta distribution, return chi square probability

double analysis::beta()
{
  // determine parameters for the beta distribution
  // mean = a/(a+b) en var = ab/((a+b)^2(a+b+1)), so
  double x = (1-h -> mean())/h -> mean();
  double a = x / (pow((1+x),3) * h -> variance()) - 1 / (1+x);
  double b = x*a;

  return beta(a,b);
}


// fit beta distribution with the given parameters, return chi square 
// probability

double analysis::beta(double a,double b)
{
  int nc = h -> columns();

  // fill expected and get chi square probability
  double* expected = new double[nc];
  double probj;
  for (int i=0;i<nc;i++)
  {
    // pdf : as below
    double j = h -> startvalue() + (i + 0.5) * h -> widthcolumn();
    if ( (j>=0) && (j<=1) )
      probj = exp(lngamma(a+b)) / exp(lngamma(a)) / exp(lngamma(b)) * pow(j,a-1) * pow(1-j,b-1);
    else
      probj = 0;
    expected[i] = h -> sumfrequencies() * probj * h -> widthcolumn();
  }
  int dof;double chisq;
  double p = chisquaretest(expected,dof,chisq);
  delete expected;

  ou -> beta(a,b,dof,chisq,p);
  cout << (*ou);
  return p;
}


// fit F distribution, return chi square probability

double analysis::fdistribution()
{
  // determine parameters for the f distribution
  // mean = m/(m+2), var = 2*m^2(n+m-2)/(n(m-2)^2(m-4))
  double l = -2*h -> mean() / (1 - h -> mean());
  unsigned int m;
  if ( (l - (int)l) > 0.5 )
    m = (unsigned int)ceil(l);
  else
    m = (unsigned int)floor(l);
  unsigned int x = 2*m*m;
  unsigned int y = 2*m*m*m - 4*m*m;
  unsigned int z = (m-2)*(m-2)*(m-4);
  double k = y / (z*h -> variance() - x);
  unsigned int n;
  if ( (k - (int)k) > 0.5 )
    n = (unsigned int)ceil(k);
  else
    n = (unsigned int)floor(k);

  return fdistribution(n,m);
}


// fit F distribution with the given parameters, return chi square 
// probability

double analysis::fdistribution(unsigned int n,unsigned int m)
{
  int nc = h -> columns();

  // fill expected and get chi square probability
  double* expected = new double[nc];
  for (int i=0;i<nc;i++)
  {
    // pdf : as below
    double j = h -> startvalue() + (i + 0.5) * h -> widthcolumn();
    if (j >= 0)
    {
      double probj = exp(lngamma((n+m)*0.5)) * pow((double)n/(double)m,n*0.5) / exp(lngamma(n*0.5)) / exp(lngamma(m*0.5)) * pow(j,n*0.5-1) * pow(1+n*j/m,(n+m)*-0.5);
      expected[i] = h -> sumfrequencies() * probj * h -> widthcolumn();
    }
    else
      expected[i] = 0;
  }
  int dof;double chisq;
  double p = chisquaretest(expected,dof,chisq);
  delete expected;

  ou -> fdistribution(n,m,dof,chisq,p);
  cout << (*ou);
  return p;
}


// fit lognormal distribution, return chi square probability

double analysis::lognormal()
{
  // determine parameters for the lognormal distribution
  // mean = e^(a+b^2/2) and variance = e^(2a+b^2)*(e^(b^2)-1), so
  double v = log((h -> variance() + h -> mean()*h -> mean()) / (h -> mean()*h -> mean()));
  double a = log(h -> mean()) - v*0.5;
  double b = sqrt(v);

  return lognormal(a,b);
}


// fit lognormal distribution with the given parameters, return chi 
// square probability

double analysis::lognormal(double a,double b)
{
  int nc = h -> columns();

  // fill expected and get chi square probability
  double* expected = new double[nc];
  for (int i=0;i<nc;i++)
  {
    // pdf : as below
    double j = h -> startvalue() + (i + 0.5) * h -> widthcolumn();
    if (j >= 0)
    {
      double probj = exp(-(log(j)-a)*(log(j)-a)/2/b/b) / sqrt(2*PI) / b / j;
      expected[i] = h -> sumfrequencies() * probj * h -> widthcolumn();
    }
    else
      expected[i] = 0;
  }
  int dof;double chisq;
  double p = chisquaretest(expected,dof,chisq);
  delete expected;

  ou -> lognormal(a,b,dof,chisq,p);
  cout << (*ou);
  return p;
}


// fit laplace distribution, return chi square probability

double analysis::laplace() 
{
  // determine parameters for the laplace distribution,
  // mean = 0 and variance = 2 * (a^2)
  double a = sqrt(0.5 * h -> variance());

  return laplace(a);
}


// fit laplace distribution with the given parameter, return chi square
// probability

double analysis::laplace(double a) 
{
  // fill expected and get chi square probability
  int nc = h -> columns();
  double* expected = new double[nc];
  for (int i=0;i<nc;i++)
  {
    // pdf : 1/(2*a) * exp(-abs(x)/a)
    double j = h -> startvalue() + (i + 0.5) * h -> widthcolumn();
    double probj = 1/(2*a) * exp(-fabs(j)/a);
    expected[i] = h -> sumfrequencies() * probj * h -> widthcolumn();
  }
  int dof;double chisq;
  double p = chisquaretest(expected,dof,chisq);
  delete expected;

  ou -> laplace(a,dof,chisq,p);
  cout << (*ou);
  return p;
}


// fit chi square distribution, return chi square probability

double analysis::chisquare() 
{
  // determine parameters for the chisquare distribution,
  // mean = n and variance = 2n, where n is a positive integer
  unsigned int n;
  if ( (h -> mean() - (int)h -> mean()) > 0.5 )
    n = (unsigned int)ceil(h -> mean());
  else
    n = (unsigned int)floor(h -> mean());

  return chisquare(n);
}


// fit chi square distribution with the given parameter, return chi 
// square probability

double analysis::chisquare(unsigned int n) 
{
  // fill expected and get chi square probability
  int nc = h -> columns();
  double* expected = new double[nc];
  for (int i=0;i<nc;i++)
  {
    // pdf : as below
    double j = h -> startvalue() + (i + 0.5) * h -> widthcolumn();
    if (j >= 0)
    {
      double probj = pow(j,n*0.5-1) * exp(-j*0.5) / pow(2,n*0.5) / exp(lngamma(n*0.5));
      expected[i] = h -> sumfrequencies() * probj * h -> widthcolumn();
    }
    else
      expected[i] = 0;
  }
  int dof;double chisq;
  double p = chisquaretest(expected,dof,chisq);
  delete expected;

  ou -> chisquare(n,dof,chisq,p);
  cout << (*ou);
  return p;
}


// fit student's t distribution, return chi square probability

double analysis::student() 
{
  // determine parameters for the student's t distribution,
  // variance = n / (n-2), so we have 
  double m = -2 * h -> variance() / (1 - h -> variance());
  unsigned int n;
  if ( (m - (int)m) > 0.5 )
    n = (unsigned int)ceil(m);
  else
    n = (unsigned int)floor(m);

  return student(n);
}


// fit student's t distribution with the given parameter, return chi 
// square probability

double analysis::student(unsigned int n) 
{
  // fill expected and get chi square probability
  int nc = h -> columns();
  double* expected = new double[nc];
  for (int i=0;i<nc;i++)
  {
    // pdf : as below
    double j = h -> startvalue() + (i + 0.5) * h -> widthcolumn();
    double probj = exp(lngamma(n*0.5+0.5)) / sqrt(n*PI) / exp(lngamma(n*0.5)) / pow((1+j*j/n),(n+1)*0.5);
    expected[i] = h -> sumfrequencies() * probj * h -> widthcolumn();
  }
  int dof;double chisq;
  double p = chisquaretest(expected,dof,chisq);
  delete expected;

  ou -> student(n,dof,chisq,p);
  cout << (*ou);
  return p;
}


// fit triangular distribution, return chi square probability

double analysis::triangular() 
{
  // determine parameters for the triangular distribution,
  // mean = (1+a)/3 and variance = (1-a^3)/(6(1-a)), so
  double a = 3 * h -> mean() - 1;

  return triangular(a);
}


// fit triangular distribution with the given parameter, return chi 
// square probability

double analysis::triangular(double a) 
{
  // fill expected and get chi square probability
  int nc = h -> columns();
  double* expected = new double[nc];
  for (int i=0;i<nc;i++)
  {
    // pdf : 2/a*x if 0<=x<a<1, 2/(1-a)(1-x) if a<=x<=1, 0 otherwise
    double j = h -> startvalue() + (i + 0.5) * h -> widthcolumn();
    double probj;
    if ( (j < a) && (j >= 0) && (a < 1) )
      probj = 2.0/a * j;
    else if ( (j >= a) && (j <= 1) )
      probj = 2.0/(1-a) * (1-j);
    else
      probj = 0;
    expected[i] = h -> sumfrequencies() * probj * h -> widthcolumn();
  }
  int dof;double chisq;
  double p = chisquaretest(expected,dof,chisq);
  delete expected;

  ou -> triangular(a,dof,chisq,p);
  cout << (*ou);
  return p;
}


// fit weibull distribution, return chi square probability

double analysis::weibull() 
{
  // determine parameters for the weibull distribution,
  // var = a^(2/b) * gammafunction(1+2/b) - mean^2, so

  double x = h -> mean()*h -> mean() / (h -> mean()*h -> mean()+h -> variance());
  double b = 0.1;
  while ( (x >= (pow(exp(lngamma(1+0.5*2.0/b)),2) / exp(lngamma(1+2.0/b)))) && (b<1000) ) 
    b += 0.1;
  if (b == 1000)
    warn(MAXWEIBULL);
  double a = pow((h -> mean() / exp(lngamma(1+1.0/b))),b);
  
  return weibull(a,b);
}


// fit weibull distribution with the given parameters, return chi 
// square probability

double analysis::weibull(double a,double b) 
{
  // fill expected and get chi square probability
  int nc = h -> columns();
  double* expected = new double[nc];
  for (int i=0;i<nc;i++)
  {
    // pdf : b/a * x^(b-1) * exp(-(x^b)/a))
    double j = h -> startvalue() + (i + 0.5) * h -> widthcolumn();
    if (j >= 0)
    {
      double probj = b / a * pow(j,b-1) * exp(pow(j,b)/-a);
      expected[i] = h -> sumfrequencies() * probj * h -> widthcolumn();
    }
    else
      expected[i] = 0;
  }
  int dof;double chisq;
  double p = chisquaretest(expected,dof,chisq);
  delete expected;

  ou -> weibull(a,b,dof,chisq,p);
  cout << (*ou);
  return p;
}


// behavior creates a report on the behavior of the histogram to be
// analyzed and adds this report to the simulation object

int analysis::behavior(histogram* hist,int buffersize)
{
  simulation* sim = thesimulation();
  if (!sim)
    err(INITERR);
  behaviorreport* std = new behaviorreport(h,hist,buffersize);
  sim -> addbehaviorreport(std);
  return OK;
}


// MM1size checks if the mean of the histogram lies in a 95% confidence
// interval of the theoretical mean of the size of a MM1 queue with
// given arrival and service rate.

int analysis::MM1size(double lb,double mu)
{
  double lq = lb*lb / (mu*(mu-lb));
  double mn = h -> mean();

  // confidence interval
  double conf;
  if (h -> sumfrequencies() > 30.0)
    conf = confidencenormal(PERCENT95);
  else
    conf = confidencestudent(PERCENT95);

  ou -> queuevalues("MM1 size",MM1SIZE,lb,mu,lq,mn,conf);
  cout << (*ou);
  return (fabs(lq-mn) <= conf);
}


// MM1waitingtime checks if the mean of the histogram lies in a 95% 
// confidence interval of the theoretical mean of the waiting time
// of a MM1 queue with given arrival and service rate.

int analysis::MM1waitingtime(double lb,double mu)
{
  double wq = lb / (mu*(mu-lb));
  double mn = h -> mean();

  // confidence interval
  double conf;
  if (h -> sumfrequencies() > 30.0)
    conf = confidencenormal(PERCENT95);
  else
    conf = confidencestudent(PERCENT95);

  ou -> queuevalues("MM1 waiting time",MM1WAITING,lb,mu,wq,mn,conf);
  cout << (*ou);
  return (fabs(wq-mn) <= conf);
}


// MMcsize checks if the mean of the histogram lies in a 95% confidence
// interval of the theoretical mean of the size of a MMc queue with
// given arrival and service rate and number of service points.

int analysis::MMcsize(double lb,double mu,unsigned int c)
{
  double rho = lb / mu;
  double sum = 0;
  for (int i=0;i<c;i++)
    sum += pow(rho,i)/exp(lngamma(i+1));
  double pt = 1.0 / (sum + pow(rho,c)*c*mu/(c*mu-lb)/exp(lngamma(c+1)));
  double lq = lb*mu  * (pow(rho,c)*pt)/(exp(lngamma(c))*(c*mu-lb)*(c*mu-lb));
  double mn = h -> mean();

  // confidence interval
  double conf;
  if (h -> sumfrequencies() > 30.0)
    conf = confidencenormal(PERCENT95);
  else
    conf = confidencestudent(PERCENT95);

  ou -> queuevalues("MMc size",MMcSIZE,lb,mu,lq,mn,conf,c);
  cout << (*ou);
  return (fabs(lq-mn) <= conf);
}


// MMcwaitingtime checks if the mean of the histogram lies in a 95% 
// confidence interval of the theoretical mean of the waiting time
// of a MMc queue with given arrival and service rate and number of
// service points.

int analysis::MMcwaitingtime(double lb,double mu,unsigned int c)
{
  double rho = lb / mu;
  double sum = 0;
  for (int i=0;i<c;i++)
    sum += pow(rho,i)/exp(lngamma(i+1));
  double pt = 1.0 / (sum + pow(rho,c)*c*mu/(c*mu-lb)/exp(lngamma(c+1)));
  double wq = mu  * (pow(rho,c)*pt)/(exp(lngamma(c))*(c*mu-lb)*(c*mu-lb));
  double mn = h -> mean();

  // confidence interval
  double conf;
  if (h -> sumfrequencies() > 30.0)
    conf = confidencenormal(PERCENT95);
  else
    conf = confidencestudent(PERCENT95);

  ou -> queuevalues("MMc waiting time",MMcWAITING,lb,mu,wq,mn,conf,c);
  cout << (*ou);
  return (fabs(wq-mn) <= conf);
}


// MD1size checks if the mean of the histogram lies in a 95% confidence
// interval of the theoretical mean of the size of a MD1 queue with 
// given arrival and service rate.

int analysis::MD1size(double lb,double mu)
{
  double rho = lb/mu;
  double lq = rho*rho / (2*(1-rho));
  double mn = h -> mean();

  // confidence interval
  double conf;
  if (h -> sumfrequencies() > 30.0)
    conf = confidencenormal(PERCENT95);
  else
    conf = confidencestudent(PERCENT95);

  ou -> queuevalues("MD1 size",MD1SIZE,lb,mu,lq,mn,conf);
  cout << (*ou);
  return (fabs(lq-mn) <= conf);
}


// MD1waitingtime checks if the mean of the histogram lies in a 95% 
// confidence interval of the theoretical mean of the waiting time
// of a MD1 queue with given arrival and service rate.

int analysis::MD1waitingtime(double lb,double mu)
{
  double rho = lb/mu;
  double wq = rho*rho / (2*(1-rho)) / lb;
  double mn = h -> mean();

  // confidence interval
  double conf;
  if (h -> sumfrequencies() > 30.0)
    conf = confidencenormal(PERCENT95);
  else
    conf = confidencestudent(PERCENT95);

  ou -> queuevalues("MD1 waiting time",MD1WAITING,lb,mu,wq,mn,conf);
  cout << (*ou);
  return (fabs(wq-mn) <= conf);
}


// generate postscript

void analysis::postscript(char* fn)
{
#ifdef HUSH
  ou -> postscript(fn);
#else
  fn = fn;
#endif
}


// formats the output

screen* analysis::format(char* p,char* options)
{
#ifdef HUSH
  return ou -> format(p,parse(options));
#else
  p = p;
  options = options;
  return NULL;
#endif
}

screen* analysis::format(widget* w,char* p,char* options)
{
#ifdef HUSH
  return ou -> format(w,p,parse(options));
#else
  w = w;
  p = p;
  options = options;
  return NULL;
#endif
}

#ifdef HUSH

// parse options, returns the options for the screen

char* analysis::parse(char* options)
{
  // standard options
  int fg,bg,fc;
  fg = bg = fc = 0;
  int wd = SCREENWIDTH;
  int hg = TEXTHEIGHT*5;

  // parse
  converter* cnv = new converter(options);
  for (int i=1;i<=cnv -> argc();i++)
  {
    if ((!strcmp((*cnv)[i],"-foreground")) && (i < cnv -> argc()))
      fg = i+1; 
    else if ((!strcmp((*cnv)[i],"-background")) && (i < cnv -> argc()))
      bg = i+1; 
    else if ((!strcmp((*cnv)[i],"-plot")) && (i < cnv -> argc()))
      fc = i+1; 
    else if ((!strcmp((*cnv)[i],"-width")) && (i < cnv -> argc()))
      wd = atoi((*cnv)[i+1]); 
    else if ((!strcmp((*cnv)[i],"-height")) && (i < cnv -> argc()))
      hg = atoi((*cnv)[i+1]); 
  }
  
  // format
  if (fg)
    sprintf(textoption,"-fill %s",(*cnv)[fg]);

  if (fc)
    sprintf(fit,"-fill %s",(*cnv)[fc]);

  char* screenoptions = new char[BUFSIZ];
  if (bg)
    sprintf(screenoptions,"-width %d -height %d -worldx %d -worldy %d -background %s",wd,hg,SCREENWIDTH,TEXTHEIGHT*5,(*cnv)[bg]);
  else
    sprintf(screenoptions,"-width %d -height %d -worldx %d -worldy %d -background grey",wd,hg,SCREENWIDTH,TEXTHEIGHT*5);
  delete cnv;
  return screenoptions;
}

#endif


// return log of the gamma function

static double lngamma(double xx)
{
  double x = xx - 1.0;
  double tmp = x + 5.5;
  tmp -= (x + 0.5) * log(tmp);
  double ser = 1.0;
  for (int j = 0;j < 6;j++) 
  {
    x += 1.0;
    ser += cof[j] / x;
  }
  return (-tmp + log(2.50662827465 * ser));
} 
