#!/usr/local/bin/perl
#

$prg_dcm2pnm='../../dcmimage/apps/dcm2pnm --quiet';
$dat_test='test.dat';
$dir_images='/data/dicom-images/cabinet';
$dir_correct='/data/joergr_tmp/correct';
$dir_results='/data/joergr_tmp/results';
$zip_images='images.zip';

if ($#ARGV < 0)
{
  $start_image = "0000";
} else {
  $start_image = $ARGV[0];
}
if ($#ARGV < 1)
{
  $end_image = "9999";
} else {
  $end_image = $ARGV[1];
}

printf("\nChecking DCMTK image library for correctness\n");
printf("using images from $dir_images\n");
printf("and comparing with correct images in $dir_correct\n");
printf("(incorrect images can be found in $dir_results)\n\n");

$errors=0;

if (open(infile, $dat_test))
{
  while (<infile>)
  {
    if (($_ !~ /^#.*/)&&($_ !~ /^[ \t]*$/))
    {
      ($number, $filename, $options, $com) = split(/[\t]+/, $_, 4);
      if ($number >= $start_image && $number <= $end_image)
      {
        ($comment) = ($com =~ /^(.*)$/);
        printf("checking #$number: $comment ... ");
        if ($options eq '-')
        {
          `$prg_dcm2pnm $dir_images/$filename $dir_results/test$number.ppm`;
        } else {
          `$prg_dcm2pnm $options $dir_images/$filename $dir_results/test$number.ppm`;
        }
        if ($? eq 0)
        {
        	printf("unzipping ... ");
        	`unzip -qqo $dir_correct/$zip_images test$number.ppm -d $dir_correct`;
          if ($? eq 0)
          {
            `cmp -s $dir_correct/test$number.ppm $dir_results/test$number.ppm`;
            if ($? eq 0)
            {
              printf("OK\n");
              `rm $dir_correct/test$number.ppm`;
              `rm $dir_results/test$number.ppm`;
            } else {
          	printf("ERROR (cmp)\n");
        	    $errors += 1
        	  }
          } else {
      	  printf("ERROR (zip)\n");
        	}
        } else {
     	    printf("ERROR (dcm2pnm)\n");
        }
      }
    } # if line no comment
  } # while infile
  close(infile);
} else {
  printf("cannot open file $dat_test\n");
}

printf("\n$errors error(s)\n\n");
