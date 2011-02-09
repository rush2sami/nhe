////////////////////////////////////////////////////////////////////////
//                                                                    //
// generator.c                                                        //
//                                                                    //
// the implementation of the generator class, it provides members for //
// creating and transforming a random number stream.                  //
//                                                                    //
// (c) 1995 D. Bolier, A. Eliens                                      //
//                                                                    //
////////////////////////////////////////////////////////////////////////


#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <limits.h>
#include <sim/generator.h>
#include <sim/error.h>
#include <sim/simulation.h>


static double lngamma(double xx);

static double cof[6]   = {76.18009173, -86.50532033, 24.01409822, -1.231739516,0.120858003e-2, -0.536382e-5};

#ifndef PI
#define PI   3.14159265358979323846
#endif


// constructor, taking the seeds of the stream

generator::generator(unsigned int s1,unsigned int s2,unsigned int s3)
{
  if ( (s1 <= 0) || (s2 < 0) || (s3 < 0) || (s1 > 30000) || (s2 > 30000) || (s3 > 30000) )
    err(SEED);
  else if ( (s2 == 0) && (s3 != 0) )
    err(SEED);
  else if ( (s2 != 0) && (s3 == 0) )
    err(SEED);
  else if ( (s2 == 0) && (s3 == 0) )
    s2 = s3 = s1;

  antithc = 0;
  x = initial_x = s1;
  y = initial_y = s2;
  z = initial_z = s3;
} 


// if antithetic is invoked the reverse of the current stream is used

void generator::antithetic()
{
  if (!antithc)
    antithc = 1;
  else
    antithc = 0;
}


// reset the current values to the initial ones

void generator::reset()
{
  x = initial_x;
  y = initial_y;
  z = initial_z;
  antithc = 0;
}


// returns a random number between 0 and 1. Every method of the
// generator class uses this method. It uses 3 generators and
// combines the results

double generator::random()
{
  x = 171 * (x%177) - 2*(x/177); 
  if (x < 0)
    x += 30269;
  y = 172 * (y%176) - 35*(y/176); 
  if (y < 0)
    y += 30307;
  z = 170 * (z%178) - 63*(z/178); 
  if (z < 0)
    z += 30323;
  double temp = (double)x/30269.0 + (double)y/30307.0 + (double)z/30323.0;
  double random = temp - (int)temp;
  if (antithc)
    return (1.0 - random);  // the reverse
  else
    return random;
} 


// generate a uniform random number between a and b

double generator::uniform(double a,double b)
{
  if (b < a) 
    err(ERRUNIFORM);
  return (random()*(b-a)+a);
} 


// return a negative exponentially distributed random number with
// the mean as parameter

double generator::exponential(double a)
{
  if (a < 0)
    err(ERREXPONENT);
  else
    return (a * (-log(random())));
}


// return a hyperexponentially distributed random number with
// the means as parameters to two exponentially distributed variates,
// the first with a chance of p of being generated, the second with a
// chance of 1-p.

double generator::hyperexponential(double p,double a,double b)
{
  if ( (a < 0) || (b < 0) )
    err(ERREXPONENT);
  else if ( (p <= 0) || (p >= 1) )
    err(PROBVAL);
  else if (probability(p))
    return (a * (-log(random())));
  else
    return (b * (-log(random())));
}


// return a laplace number with the given parameter

double generator::laplace(double a)
{
  if (probability(0.5))
    return (a * ( -log(random()) ));
  else
    return (a * ( log(random()) ));
}


// return random variable from the erlang distribution

double generator::erlang(unsigned int n,double a)
{
  if (a < 0)
    err(ERRERLANG);
  else 
  {
    // use gamma distribution
    return (gamma((double)n,a));
  }
}


// return random number from the gamma distribution

double generator::gamma(double a,double b)
{
  // rejection method only for a > 1
  double xx,avg,am,e,s,v1,v2,yy;
  if ((a < 0.0) || (b < 0.0))
    err(ERRGAMMA);

  if (a < 1.0) 
  {
    do
    {
      xx = pow(random(),1.0/a);
      yy = pow(random(),1.0/(1.0-a));
    }
    while (xx+yy > 1.0);

    xx = xx/(xx+yy);
    yy = exponential(1);
    return xx*yy/b;
  }

  if (a == 1.0)
    return (exponential (1) / b);

  // rejection method
  do {
    do {
      do {
        v1 = 2.0 * random() - 1.0;
        v2 = 2.0 * random() - 1.0;
      } while (v1 * v1 + v2 * v2 > 1.0);

      yy = v2 / v1;
      am = a - 1.0;
      s = sqrt(2.0 * am + 1.0);
      avg = s * yy + am;
    } while (avg <= 0.0);
    e = (1.0 + yy * yy) * exp(am * log(avg / am) - s * yy);
  } while (random() > e);

  return avg / b ;
}


// return a normally distributed random number with given mean and
// standard deviation

double generator::normal(double a,double b)
{
  double s,u,v1,v2,x1;
  if (b < 0.0) 
    err(ERRNORMAL);
  do
  {
    u  = random();
    v1 = 2.0 * u  - 1.0;
    u  = random();
    v2 = 2.0 * random() - 1.0;
    s = v1 * v1 + v2 * v2;
  } while (s >= 1.0 || s == 0.0);

  x1 = v1 * sqrt((-2.0 * log(s)) / s);
  return (a + x1 * b);
} 


// return a random variable from a chi square distribution with
// n degrees of freedom

double generator::chisquare(unsigned int n)
{
  return gamma(0.5*n,0.5);
}


// return random variabele from a beta distribution

double generator::beta(double a,double b)
{
  if (b == 0.0)
    err(ERRBETA);
  double zz = gamma(a,1.0);
  return (zz/(zz+gamma(b,1.0)));
}


// return it from the student distribution

double generator::student(unsigned int n)
{
  return (normal(0.0,1.0)/sqrt(chisquare(n)/n));
} 


// return a random number from a lognormal distribution

double generator::lognormal(double a,double b)
{
  if (b < 0.0) 
    err(ERRLOGNORM);
  return exp(normal(a,b));
} 


// return it from a f distribution.

double generator::fdistribution(unsigned int n,unsigned int m)
{
  return ( (m * chisquare(n)) / (n * chisquare(m)) );
}


// random number from a weibull distribution

double generator::weibull (double a,double b)
{
  return pow(exponential(a),(1.0/b));
} 


// get it from a poisson distribution with the given mean

double generator::poisson(double a)
{
  // can we use the direct method
  double sq,alxm,g,oldm = -1.0;
  double em,t,yy;
  if (a < 12.0) 
  {
    if (a != oldm) 
    {
      oldm = a;
      g = exp(-a);
    }
    em = -1.0;
    t = 1.0;
    do 
    {
      em += 1.0;
      t *= random();
    } while (t > g);
  }

  // use the rejection method
  else 
  {
    if (a != oldm) 
    {
      oldm = a;
      sq = sqrt(2.0 * a);
      alxm = log(a);
      g = a*alxm-lngamma(a + 1.0);
    }
    do 
    {
      do 
      {
        // y is a deviate from a Lorentzian comparison function
        yy = tan (PI * random());
        em = sq * yy + a;
      } while (em < 0.0);
      em = floor(em);
      t = 0.9 * (1.0 + yy * yy) * exp(em * alxm - lngamma(em + 1.0) - g);
    } while (random() > t);
  }
  return em;
} 


// return a variate from the geometric distribution with event
// probability p

double generator::geometric(double p)
{
  if ((p <= 0.0) || (p >= 1.0))
    err(PROBVAL);
  return (ceil(log(random())/log(1.0 - p)));
}


// return a variate from the hypergeometric distribution with m the
// population, p the chance on success and n the number of items drawn

double generator::hypergeometric(unsigned int m,unsigned int n,double p)
{
  if ((p <= 0.0) || (p >= 1.0))
    err(PROBVAL);
  else if (m < n)
    err(ERRHYPGEO);
  double g = p*m;              // the success items
  double c = m-g;              // the non-success items
  int k=0;
  for (int i=0;i<n;i++)        // n drawings
  {
    if (random() <= g/m)       // if a success
    {
      k++;                     // success
      g--;                     // decrease right items
    }
    else
      c--;                     // decrease wrong items
    m--;                       // decrease total
  }
  return k;                    // return deviate
}
  

// get it from the binomial distribution with event probability p
// and n the number of trials

double generator::binomial(double p,unsigned int n)
{
  int j;
  int nold = -1;
  double am,em,g,angle,prob,bnl,sq,t,yy;
  double pold = -1.0;
  double pc,plog,pclog,en,oldg;

  if ((p <= 0.0) || (p >= 1.0))
    err(PROBVAL);

  if (p <= 0.5 )
    prob = p ;
  else
    prob = 1.0 - p ;
  am = n * prob;

  if (n < 25)
  {
    bnl = 0.0;
    for (j = 1;j <= n;j++)
      if (random() < prob)
        bnl += 1.0;
  }
  else if (am < 10)
  {
    g = exp(-am);
    t = 1.0;
    for (j = 0;j <= n;j++) 
    {
      t = t * random();
      if (t < g) break;
    }
    if (j <= n)
      bnl = j;
    else
      bnl = n;
  }
  else 
  {
    if (n != nold) 
    {
      en = n;
      oldg = lngamma(en + 1.0);
      nold = n;
    }
    if (prob != pold) 
    {
      pc = 1.0 - prob;
      plog = log(prob);
      pclog = log(pc);
      pold = prob;
    }
    sq = sqrt( 2.0 * am * pc );
    do 
    {
      do 
      {
        angle = PI * random();
        yy = tan(angle);
        em = sq * yy + am;

      } while (em < 0.0 || em >= (en + 1.0));
      em = floor(em);
      t = 1.2 * sq * (1.0 + yy * yy) * exp(oldg - lngamma(em + 1.0)
            - lngamma(en - em + 1.0) + em * plog + (en - em) * pclog);
    } while (random() > t);
    bnl = em;
  }
  if (prob != p)
    bnl = n - bnl;
  return bnl;
}


// return a random variabele with probabilty of success equal to p
// and n as the number of successes

double generator::negativebinomial(double p,unsigned int n)
{
  if ((p <= 0.0) || (p >= 1.0))
    err(PROBVAL);
  return (poisson(gamma(n,p/(1.0-p))));
} 


// return a random variabele drawn from the triangular distribution

double generator::triangular(double a)
{
  if ((a < 0.0) || (a > 1.0)) 
    err(ERRTRIANG);
  double xx = random();
  double yy = random();
  if (xx > a) 
    return ( 1.0 - (( 1.0 - a) * sqrt(yy)) );
  else
    return (sqrt(yy) * a);
} 


// return OK with probability p and FALSE with probability 1-p

int generator::probability(double p)
{
  if ((p < 0.0) || (p > 1.0)) 
    err(PROBVAL);
  return (p >= random());
}


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
