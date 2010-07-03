"Turner, Scott" <Scott.Turner@jhuapl.edu> 
12/03/2007 03:08 PM	To
Kris J Becker <kbecker@usgs.gov>
cc
Mark Robinson <mrobinson@asu.edu>, "Prockter, Louise M." <Louise.Prockter@jhuapl.edu>
bcc

Subject
MDIS Geometric Distortion Models (Initial, rough estimate)
	
		
		
		Kris,

Here are the initial distortion model coefficients based on my analysis of
the star cluster mosaic images taken back in April of 2005.  I opted to
provide effective focal lengths (setting k1 in the model to 0), because it
was convenient at the time in my code.  We can adjust them later if you
prefer a calibrated focal length instead, as Randy suggested was common
practice. The error bars are one-sigma, assuming a Gaussian profile to the
measurement error--which may or may not be appropriate.

WAC:

   f = 78.177 mm   +/- 0.0041
  k1 =  0.0
  k3 =  1.6520E-5  +/- 2.2637E-06
  k5 = -1.2035E-7  +/- 2.5833E-08

NAC: 

   f = 549.273 mm  +/- 0.5263
  k1 =  0.0
  k3 =  1.6902E-5  +/- 5.1195E-05
  k5 = -5.4783E-7  +/- 5.6724E-07

Clearly the NAC fit results are worse, but this is to be expected given that
we are data starved per exposure by comparison to the WAC.  A couple of
other points worth mentioning, as we discussed last week I removed the
(x0,y0) parameters from the modeling effort and simply set them both to 0.0.
If you have time to include them in your implementation in ISIS, their
inclusion beats down the residuals a noticeable amount.

And, speaking of residuals, for reference, the residuals for the WAC
regression are roughly in the 0.18 to 0.25 range (RMS average/exposure).
And the NAC residuals are a bit worse, ranging from 0.29 to 0.76 RMS
average/exposure.

A few other observations, in my modeling efforts I have noticed that the
point spread function for the NAC appears to be about 1.6-1.7 times broader
than that of the WAC.  I seem to recall some discussion about this with Mark
a year or so ago, but only in so much as the NAC's was more broad.  I'm not
sure if this particular value was to be expected or not.  Also, I have
observed that the alignment uncertainties in the fits are about an order of
magnitude or so higher for the NAC images.  However, I was expecting this,
as though it is much higher resolution we have far fewer stars per exposure.

Once you have things up and running, if you happen to have any insights into
the accuracy of these numbers please share them with me.  I am still going
to continue trying to reduce/interpret the data to resolve some of the
issues with the values I have provided.

Feel free to call or shoot me an email if you have any questions or concerns
about what I have provided here.  Thanks again for all of your patience,

Scott

PS: I was going to put the parameters into an update to the I-kernel
addendum you created, but figured rather than make you wait until tomorrow I
would just send the numbers along.  If you still want me to capture them in
the text kernel format, let me know.  Otherwise, pass back the updated one
you create and I'll see that it gets incorporated into the actual I-kernel
released on the SOC webserver (http://bronte.jhuapl.edu).
