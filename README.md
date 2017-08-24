# DTI Fiber Tract Statistics

## What is it?

This is a software which allows the user to study the behavior of water diffusion (using DTI data) along the length of the white matter fiber-tracts.
Various tract-oriented scalar diffusion measures obtained from DTI brain images, are treated as a continuous function of white matter fibers' arc-length. To analyze the trend along a given fiber tract, a command line tool performs kernel regression on this data. The idea is to try out different noise models and maximum likelihood estimates within kernel windows (along the tract), such that they best represent the data and are robust to noise and Partial Volume effect.

The package contains several command line based modules and an GUI based tool called DTIAtlasFiberAnalyzer to access most functions.

This package is also available as a 3D Slicer extension (http://www.slicer.org) called DTIAtlasFiberAnalyzer.

Some documentation about the features available in this package are available here: http://www.na-mic.org/Wiki/index.php/Projects:dtistatisticsfibers

## License

See License.txt

## More information

Find the tool on [NITRC](https://www.nitrc.org/projects/dti_tract_stat)

