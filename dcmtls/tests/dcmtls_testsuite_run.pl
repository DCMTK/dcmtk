#! /usr/bin/perl
#
#  Copyright (C) 2018, OFFIS e.V.
#  All rights reserved.  See COPYRIGHT file for details.
#
#  This software and supporting documentation were developed by
#
#    OFFIS e.V.
#    R&D Division Health
#    Escherweg 2
#    D-26121 Oldenburg, Germany
#
#
#  Module: dcmtls
#
#  Author: Marco Eichelberg
#
#  Purpose:
#    This script is a temporary test suite for module dcmtls that
#    exercises all options of the TLS code in dcmtk that can be reached
#    through echoscu and storescp. This script will later be replaced
#    with a solution based on CMake/CTest.
#

use File::Copy ('copy');
use Digest::MD5;

# determine if we're running on Windows
if ($^O =~ /MSWin32/) { $windows = 1; } else { $windows = 0; }

$tlsport = 10004;
$dcmtk_bin_path = "~/dicom/out3/bin";
$echoscu = "$dcmtk_bin_path/echoscu";
$storescp = "$dcmtk_bin_path/storescp";
$tcpclient = "~/dicom/tests/drivers/apps/tcpclient";

$numPassed = 0;
$numFailed = 0;

# Determine and print OpenSSL version DCMTK was compiled with
system("$echoscu --version >echoscu_version.log");
open(ECHOSCUFILE, "<echoscu_version.log") || die "unable to open file 'echoscu_version.log'";
$with_openssl = 0;
while ($row = <ECHOSCUFILE>)
{
  chomp $row;
  if ($row =~ /OpenSSL/) { printf("OpenSSL version: %s\n\n", substr($row,2)); $with_openssl = 1; }
}
close INFILE;
if (! $with_openssl)
{
  print "DCMTK compiled without OpenSSL support. Stopping dcmtls test suite\n";
  exit 0;
}

print "Test 01: unencrypted communication: ";
$pid = &startBackgroundProcess("$storescp -d $tlsport >test01_server_out.log 2>test01_server_err.log");
do { $result = system("$tcpclient localhost $tlsport >tcpclient.log"); } while ($result != 0);
$result = system("$echoscu localhost -d $tlsport >test01_client_out.log 2>test01_client_err.log");
if ($result == 0) { &testPassed(); } else { &testFailed(); }
&doKill($pid);
$tlsport++;

print "Test 02: attempt TLS connection to unencrypted server: ";
$pid = &startBackgroundProcess("$storescp -d $tlsport >test02_server_out.log 2>test02_server_err.log");
do { $result = system("$tcpclient localhost $tlsport >tcpclient.log"); } while ($result != 0);
$result = system("$echoscu -d +tla localhost $tlsport >test02_client_out.log 2>test02_client_err.log");
if (($result != 0) && (&findInFiles("Unrecognized PDU type", "test02_server_err.log", "test02_server_out.log"))) { &testPassed(); } else { &testFailed(); }
&doKill($pid);
$tlsport++;

print "Test 03: attempt unencrypted connection to TLS server: ";
$pid = &startBackgroundProcess("$storescp -d +tls user1_key.pem user1_cert.pem $tlsport >test03_server_out.log 2>test03_server_err.log");
do { $result = system("$tcpclient localhost $tlsport >tcpclient.log"); } while ($result != 0);
$result = system("$echoscu -d localhost $tlsport >test03_client_out.log 2>test03_client_err.log");
# different openssl versions report different error messages here
if (($result != 0) && (
  (&findInFiles("wrong version number", "test03_server_err.log", "test03_server_out.log")) ||
  (&findInFiles("unknown protocol", "test03_server_err.log", "test03_server_out.log")))) { &testPassed(); } else { &testFailed(); }
&doKill($pid);
$tlsport++;

print "Test 04: connect with anonymous TLS to TLS server accepting such connections: ";
$pid = &startBackgroundProcess("$storescp -d +tls user1_key.pem user1_cert.pem -ic $tlsport >test04_server_out.log 2>test04_server_err.log");
do { $result = system("$tcpclient localhost $tlsport >tcpclient.log"); } while ($result != 0);
$result = system("$echoscu -d +tla -ic localhost $tlsport >test04_client_out.log 2>test04_client_err.log");
if ($result == 0) { &testPassed(); } else { &testFailed(); }
&doKill($pid);
$tlsport++;

print "Test 05: connect with valid certificate to TLS server: ";
$pid = &startBackgroundProcess("$storescp -d +tls user1_key.pem user1_cert.pem -ic $tlsport >test05_server_out.log 2>test05_server_err.log");
do { $result = system("$tcpclient localhost $tlsport >tcpclient.log"); } while ($result != 0);
$result = system("$echoscu -d +tls user1_key.pem user1_cert.pem -ic localhost $tlsport >test05_client_out.log 2>test05_client_err.log");
if ($result == 0) { &testPassed(); } else { &testFailed(); }
&doKill($pid);
$tlsport++;

print "Test 06: connect with weak certificate (MD5 hash) to TLS server: ";
$pid = &startBackgroundProcess("$storescp -d +tls user1_key.pem user1_cert.pem -ic +cf cacert1.pem $tlsport >test06_server_out.log 2>test06_server_err.log");
do { $result = system("$tcpclient localhost $tlsport >tcpclient.log"); } while ($result != 0);
$result = system("$echoscu -d +tls user5_key.pem user5_cert.pem -ic localhost $tlsport >test06_client_out.log 2>test06_client_err.log");
# here we do not look at the return code because the result differs depending on the openssl version used.
# openssl 1.0.x will report a warning but run echoscu, while 1.1.x will refuse to use the certificate.
# The warning is printed in both cases, however, and this is what we look for.
if (&findInFiles("Certificate hash key not SHA-256", "test06_client_err.log", "test06_client_out.log")) { &testPassed(); } else { &testFailed(); }
&doKill($pid);
$tlsport++;

print "Test 07: connect with DER encoded certificate to TLS server: ";
$pid = &startBackgroundProcess("$storescp -d +tls user1_key.pem user1_cert.pem -ic $tlsport >test07_server_out.log 2>test07_server_err.log");
do { $result = system("$tcpclient localhost $tlsport >tcpclient.log"); } while ($result != 0);
$result = system("$echoscu -d -der +tls user2_key.der user2_cert.der -ic localhost $tlsport >test07_client_out.log 2>test07_client_err.log");
if ($result == 0) { &testPassed(); } else { &testFailed(); }
&doKill($pid);
$tlsport++;

print "Test 08: use private key file protected with a password: ";
$pid = &startBackgroundProcess("$storescp -d +tls user1_key.pem user1_cert.pem -ic $tlsport >test08_server_out.log 2>test08_server_err.log");
do { $result = system("$tcpclient localhost $tlsport >tcpclient.log"); } while ($result != 0);
$result = system("$echoscu -d +tls user3_key.pem user3_cert.pem +pw PASSWORD -ic localhost $tlsport >test08_client_out.log 2>test08_client_err.log");
if ($result == 0) { &testPassed(); } else { &testFailed(); }
&doKill($pid);
$tlsport++;

print "Test 09: use private key file protected with a password; use wrong password: ";
$pid = &startBackgroundProcess("$storescp -d +tls user1_key.pem user1_cert.pem -ic $tlsport >test09_server_out.log 2>test09_server_err.log");
do { $result = system("$tcpclient localhost $tlsport >tcpclient.log"); } while ($result != 0);
$result = system("$echoscu -d +tls user3_key.pem user3_cert.pem +pw WRONG_PASSWORD -ic localhost $tlsport >test09_client_out.log 2>test09_client_err.log");
if (($result != 0) && (&findInFiles("Unable to load private TLS key", "test09_client_err.log", "test09_client_out.log"))) { &testPassed(); } else { &testFailed(); }
&doKill($pid);
$tlsport++;

print "Test 10: use private key file protected with a password; use null password: ";
$pid = &startBackgroundProcess("$storescp -d +tls user1_key.pem user1_cert.pem -ic $tlsport >test10_server_out.log 2>test10_server_err.log");
do { $result = system("$tcpclient localhost $tlsport >tcpclient.log"); } while ($result != 0);
$result = system("$echoscu -d +tls user3_key.pem user3_cert.pem --null-passwd -ic localhost $tlsport >test10_client_out.log 2>test10_client_err.log");
if (($result != 0) && (&findInFiles("Unable to load private TLS key", "test10_client_err.log", "test10_client_out.log"))) { &testPassed(); } else { &testFailed(); }
&doKill($pid);
$tlsport++;

print "Test 11: use random seed file: ";
copy("randseed.bin", "randseed11.bin") || die "unable to copy randseed.bin";
$pid = &startBackgroundProcess("$storescp -d +tls user1_key.pem user1_cert.pem -ic $tlsport >test11_server_out.log 2>test11_server_err.log");
do { $result = system("$tcpclient localhost $tlsport >tcpclient.log"); } while ($result != 0);
$result = system("$echoscu -d --seed randseed11.bin +tls user1_key.pem user1_cert.pem -ic localhost $tlsport >test11_client_out.log 2>test11_client_err.log");
$digest1 = md5digest("randseed.bin");
$digest2 = md5digest("randseed11.bin");
unlink "randseed11.bin";
if ($result == 0 && ($digest1 eq $digest2)) { &testPassed(); } else { &testFailed(); }
&doKill($pid);
$tlsport++;

print "Test 12: use and write back random seed file: ";
copy("randseed.bin", "randseed12.bin") || die "unable to copy randseed.bin";
$pid = &startBackgroundProcess("$storescp -d +tls user1_key.pem user1_cert.pem -ic $tlsport >test12_server_out.log 2>test12_server_err.log");
do { $result = system("$tcpclient localhost $tlsport >tcpclient.log"); } while ($result != 0);
$result = system("$echoscu -d --seed randseed12.bin --write-seed +tls user1_key.pem user1_cert.pem -ic localhost $tlsport >test12_client_out.log 2>test12_client_err.log");
$digest1 = md5digest("randseed.bin");
$digest2 = md5digest("randseed12.bin");
unlink "randseed12.bin";
if ($result == 0 && ($digest1 ne $digest2)) { &testPassed(); } else { &testFailed(); }
&doKill($pid);
$tlsport++;

print "Test 13: use random seed file, write back to different file: ";
copy("randseed.bin", "randseed13.bin") || die "unable to copy randseed.bin";
$pid = &startBackgroundProcess("$storescp -d +tls user1_key.pem user1_cert.pem -ic $tlsport >test13_server_out.log 2>test13_server_err.log");
do { $result = system("$tcpclient localhost $tlsport >tcpclient.log"); } while ($result != 0);
unlink "randseed13a.bin";
$result = system("$echoscu -d --seed randseed13.bin +wf randseed13a.bin +tls user1_key.pem user1_cert.pem -ic localhost $tlsport >test13_client_out.log 2>test13_client_err.log");
if (! -f "randseed13a.bin")
{ # output file was not created. This is a failure
  &testFailed();
}
else
{
  $digest1 = md5digest("randseed13.bin");
  $digest2 = md5digest("randseed13a.bin");
  unlink "randseed13.bin";
  unlink "randseed13a.bin";
  if ($result == 0 && ($digest1 ne $digest2)) { &testPassed(); } else { &testFailed(); }
}
&doKill($pid);
$tlsport++;

print "Test 14: client verifies server certificate: ";
$pid = &startBackgroundProcess("$storescp -d +tls user1_key.pem user1_cert.pem -ic $tlsport >test14_server_out.log 2>test14_server_err.log");
do { $result = system("$tcpclient localhost $tlsport >tcpclient.log"); } while ($result != 0);
$result = system("$echoscu -d +tls user1_key.pem user1_cert.pem -rc +cf cacert1.pem localhost $tlsport >test14_client_out.log 2>test14_client_err.log");
if ($result == 0) { &testPassed(); } else { &testFailed(); }
&doKill($pid);
$tlsport++;

print "Test 15: server provides certificate from unknown CA: ";
$pid = &startBackgroundProcess("$storescp -d +tls ca2user_key.pem ca2user_cert.pem -ic $tlsport >test15_server_out.log 2>test15_server_err.log");
do { $result = system("$tcpclient localhost $tlsport >tcpclient.log"); } while ($result != 0);
$result = system("$echoscu -d +tls user1_key.pem user1_cert.pem -rc +cf cacert1.pem localhost $tlsport >test15_client_out.log 2>test15_client_err.log");
if (($result != 0) && (&findInFiles("certificate verify failed", "test15_client_err.log", "test15_client_out.log"))) { &testPassed(); } else { &testFailed(); }
&doKill($pid);
$tlsport++;

print "Test 16: server verifies client certificate: ";
$pid = &startBackgroundProcess("$storescp -d +tls user1_key.pem user1_cert.pem -rc +cf cacert1.pem $tlsport >test16_server_out.log 2>test16_server_err.log");
do { $result = system("$tcpclient localhost $tlsport >tcpclient.log"); } while ($result != 0);
$result = system("$echoscu -d +tls user1_key.pem user1_cert.pem -rc +cf cacert1.pem localhost $tlsport >test16_client_out.log 2>test16_client_err.log");
if ($result == 0) { &testPassed(); } else { &testFailed(); }
&doKill($pid);
$tlsport++;

print "Test 17: server tries to verify client certificate, but client uses anonymous TLS: ";
$pid = &startBackgroundProcess("$storescp -d +tls user1_key.pem user1_cert.pem -rc +cf cacert1.pem $tlsport >test17_server_out.log 2>test17_server_err.log");
do { $result = system("$tcpclient localhost $tlsport >tcpclient.log"); } while ($result != 0);
$result = system("$echoscu -d +tla -rc +cf cacert1.pem localhost $tlsport >test17_client_out.log 2>test17_client_err.log");
if (($result != 0) && (&findInFiles("peer did not return a certificate", "test17_server_err.log", "test17_server_out.log"))) { &testPassed(); } else { &testFailed(); }
&doKill($pid);
$tlsport++;

print "Test 18: client uses certificate from unknown CA: ";
$pid = &startBackgroundProcess("$storescp -d +tls user1_key.pem user1_cert.pem -rc +cf cacert1.pem $tlsport >test18_server_out.log 2>test18_server_err.log");
do { $result = system("$tcpclient localhost $tlsport >tcpclient.log"); } while ($result != 0);
$result = system("$echoscu -d +tls ca2user_key.pem ca2user_cert.pem -rc +cf cacert1.pem localhost $tlsport >test18_client_out.log 2>test18_client_err.log");
# different openssl versions report different error messages here
if (($result != 0) && (
  (&findInFiles("certificate verify failed", "test18_server_err.log", "test18_server_out.log")) ||
  (&findInFiles("no certificate returned", "test18_server_err.log", "test18_server_out.log")))) { &testPassed(); } else { &testFailed(); }
&doKill($pid);
$tlsport++;

print "Test 19: server verifies client certificate only if present, client uses anonymous TLS: ";
$pid = &startBackgroundProcess("$storescp -d +tls user1_key.pem user1_cert.pem -vc +cf cacert1.pem $tlsport >test19_server_out.log 2>test19_server_err.log");
do { $result = system("$tcpclient localhost $tlsport >tcpclient.log"); } while ($result != 0);
$result = system("$echoscu -d +tla -rc +cf cacert1.pem localhost $tlsport >test19_client_out.log 2>test19_client_err.log");
if ($result == 0) { &testPassed(); } else { &testFailed(); }
&doKill($pid);
$tlsport++;

print "Test 20: server verifies client certificate only if present, client sends certificate: ";
$pid = &startBackgroundProcess("$storescp -d +tls user1_key.pem user1_cert.pem -vc +cf cacert1.pem $tlsport >test20_server_out.log 2>test20_server_err.log");
do { $result = system("$tcpclient localhost $tlsport >tcpclient.log"); } while ($result != 0);
$result = system("$echoscu -d +tls user1_key.pem user1_cert.pem -rc +cf cacert1.pem localhost $tlsport >test20_client_out.log 2>test20_client_err.log");
if ($result == 0) { &testPassed(); } else { &testFailed(); }
&doKill($pid);
$tlsport++;

print "Test 21: server verifies client certificate only if present, client uses certificate from unknown CA: ";
$pid = &startBackgroundProcess("$storescp -d +tls user1_key.pem user1_cert.pem -vc +cf cacert1.pem $tlsport >test21_server_out.log 2>test21_server_err.log");
do { $result = system("$tcpclient localhost $tlsport >tcpclient.log"); } while ($result != 0);
$result = system("$echoscu -d +tls ca2user_key.pem ca2user_cert.pem -rc +cf cacert1.pem localhost $tlsport >test21_client_out.log 2>test21_client_err.log");
# different openssl versions report different error messages here
if (($result != 0) && (
  (&findInFiles("certificate verify failed", "test21_server_err.log", "test21_server_out.log")) ||
  (&findInFiles("no certificate returned", "test21_server_err.log", "test21_server_out.log")))) { &testPassed(); } else { &testFailed(); }
&doKill($pid);
$tlsport++;

print "Test 22: server uses a directory of CA certificates, client uses CA1 certificate: ";
$pid = &startBackgroundProcess("$storescp -d +tls user1_key.pem user1_cert.pem -rc +cd cadir $tlsport >test22_server_out.log 2>test22_server_err.log");
do { $result = system("$tcpclient localhost $tlsport >tcpclient.log"); } while ($result != 0);
$result = system("$echoscu -d +tls user1_key.pem user1_cert.pem  -rc +cf cacert1.pem localhost $tlsport >test22_client_out.log 2>test22_client_err.log");
if ($result == 0) { &testPassed(); } else { &testFailed(); }
&doKill($pid);
$tlsport++;

print "Test 23: server uses a directory of CA certificates, client uses CA2 certificate: ";
$pid = &startBackgroundProcess("$storescp -d +tls user1_key.pem user1_cert.pem -rc +cd cadir $tlsport >test23_server_out.log 2>test23_server_err.log");
do { $result = system("$tcpclient localhost $tlsport >tcpclient.log"); } while ($result != 0);
$result = system("$echoscu -d +tls ca2user_key.pem ca2user_cert.pem -rc +cf cacert1.pem localhost $tlsport >test23_client_out.log 2>test23_client_err.log");
if ($result == 0) { &testPassed(); } else { &testFailed(); }
&doKill($pid);
$tlsport++;

print "Test 24: server uses a directory of CA certificates, client uses certificate from unknown CA: ";
$pid = &startBackgroundProcess("$storescp -d +tls user1_key.pem user1_cert.pem -rc +cd cadir $tlsport >test24_server_out.log 2>test24_server_err.log");
do { $result = system("$tcpclient localhost $tlsport >tcpclient.log"); } while ($result != 0);
$result = system("$echoscu -d +tls ca3user_key.pem ca3user_cert.pem -rc +cf cacert1.pem localhost $tlsport >test24_client_out.log 2>test24_client_err.log");
# different openssl versions report different error messages here
if (($result != 0) && (
  (&findInFiles("certificate verify failed", "test24_server_err.log", "test24_server_out.log")) ||
  (&findInFiles("no certificate returned", "test24_server_err.log", "test24_server_out.log")))) { &testPassed(); } else { &testFailed(); }
&doKill($pid);
$tlsport++;

print "Test 25: server uses user-defined Diffie-Hellman parameter set: ";
$pid = &startBackgroundProcess("$storescp -d +tls user1_key.pem user1_cert.pem --dhparam dhparam2048.pem -ic $tlsport >test25_server_out.log 2>test25_server_err.log");
do { $result = system("$tcpclient localhost $tlsport >tcpclient.log"); } while ($result != 0);
$result = system("$echoscu -d +tls user1_key.pem user1_cert.pem -ic localhost $tlsport >test25_client_out.log 2>test25_client_err.log");
if ($result == 0) { &testPassed(); } else { &testFailed(); }
&doKill($pid);
$tlsport++;

print "Test 26: server uses weak (512 bit) user-defined Diffie-Hellman parameter set: ";
$pid = &startBackgroundProcess("$storescp -d +tls user1_key.pem user1_cert.pem --dhparam dhparam512.pem -ic $tlsport >test26_server_out.log 2>test26_server_err.log");
do { $result = system("$tcpclient localhost $tlsport >tcpclient.log"); } while ($result != 0);
$result = system("$echoscu -d +tls user1_key.pem user1_cert.pem -ic localhost $tlsport >test26_client_out.log 2>test26_client_err.log");
# here we do not look at the return code because the result differs depending on the openssl version used.
# openssl 1.1.x will report a warning but run echoscu, while 1.0.x will refuse to use the DH parameter set.
# The warning is printed in both cases, however, and this is what we look for.
if (&findInFiles("Key length of Diffie-Hellman parameter file too short", "test26_server_err.log", "test26_server_out.log")) { &testPassed(); } else { &testFailed(); }
&doKill($pid);
$tlsport++;

print "Test 27: connect with weak certificate (512-bit RSA) to TLS server: ";
$pid = &startBackgroundProcess("$storescp -d +tls user1_key.pem user1_cert.pem -ic $tlsport >test27_server_out.log 2>test27_server_err.log");
do { $result = system("$tcpclient localhost $tlsport >tcpclient.log"); } while ($result != 0);
$result = system("$echoscu -d +tls user4_key.pem user4_cert.pem -ic localhost $tlsport >test27_client_out.log 2>test27_client_err.log");
# here we do not look at the return code because the result differs depending on the openssl version used.
# openssl 1.0.x will report a warning but run echoscu, while 1.1.x will refuse to use the certificate.
# The warning is printed in both cases, however, and this is what we look for.
if (&findInFiles("Key length of RSA public key too short", "test27_client_err.log", "test27_client_out.log")) { &testPassed(); } else { &testFailed(); }
&doKill($pid);
$tlsport++;

print "Test 28: client uses Basic TLS Secure Transport Connection Profile (3DES): ";
$pid = &startBackgroundProcess("$storescp -d +tls user1_key.pem user1_cert.pem -ic $tlsport >test28_server_out.log 2>test28_server_err.log");
do { $result = system("$tcpclient localhost $tlsport >tcpclient.log"); } while ($result != 0);
$result = system("$echoscu -d +tla --profile-basic -ic localhost $tlsport >test28_client_out.log 2>test28_client_err.log");
if ($result == 0) { &testPassed(); } else { &testFailed(); }
&doKill($pid);
$tlsport++;

print "Test 29: client uses Basic Profile (3DES), server uses Non-downgrading BCP 195 TLS Profile: ";
$pid = &startBackgroundProcess("$storescp -d --profile-bcp195-nd +tls user1_key.pem user1_cert.pem -ic $tlsport >test29_server_out.log 2>test29_server_err.log");
do { $result = system("$tcpclient localhost $tlsport >tcpclient.log"); } while ($result != 0);
$result = system("$echoscu -d +tla --profile-basic -ic localhost $tlsport >test29_client_out.log 2>test29_client_err.log");
if (($result != 0) && (&findInFiles("no shared cipher", "test29_server_err.log", "test29_server_out.log"))) { &testPassed(); } else { &testFailed(); }
&doKill($pid);
$tlsport++;

print "Test 30: client uses AES TLS Secure Transport Connection Profile: ";
$pid = &startBackgroundProcess("$storescp -d +tls user1_key.pem user1_cert.pem -ic $tlsport >test30_server_out.log 2>test30_server_err.log");
do { $result = system("$tcpclient localhost $tlsport >tcpclient.log"); } while ($result != 0);
$result = system("$echoscu -d +tla --profile-aes -ic localhost $tlsport >test30_client_out.log 2>test30_client_err.log");
if ($result == 0) { &testPassed(); } else { &testFailed(); }
&doKill($pid);
$tlsport++;

print "Test 31: client uses AES Profile, server uses Non-downgrading BCP 195 TLS Profile: ";
$pid = &startBackgroundProcess("$storescp -d --profile-bcp195-nd +tls user1_key.pem user1_cert.pem -ic $tlsport >test31_server_out.log 2>test31_server_err.log");
do { $result = system("$tcpclient localhost $tlsport >tcpclient.log"); } while ($result != 0);
$result = system("$echoscu -d +tla --profile-aes -ic localhost $tlsport >test31_client_out.log 2>test31_client_err.log");
if (($result != 0) && (&findInFiles("no shared cipher", "test31_server_err.log", "test31_server_out.log"))) { &testPassed(); } else { &testFailed(); }
&doKill($pid);
$tlsport++;

print "Test 32: client uses BCP 195 Profile, server uses Non-downgrading BCP 195 TLS Profile: ";
$pid = &startBackgroundProcess("$storescp -d --profile-bcp195-nd +tls user1_key.pem user1_cert.pem -ic $tlsport >test32_server_out.log 2>test32_server_err.log");
do { $result = system("$tcpclient localhost $tlsport >tcpclient.log"); } while ($result != 0);
$result = system("$echoscu -d +tla --profile-bcp195 -ic localhost $tlsport >test32_client_out.log 2>test32_client_err.log");
if ($result == 0) { &testPassed(); } else { &testFailed(); }
&doKill($pid);
$tlsport++;

print "Test 33: client uses Non-downgrading BCP 195 TLS Profile, server uses BCP 195 Profile: ";
$pid = &startBackgroundProcess("$storescp -d --profile-bcp195 +tls user1_key.pem user1_cert.pem -ic $tlsport >test33_server_out.log 2>test33_server_err.log");
do { $result = system("$tcpclient localhost $tlsport >tcpclient.log"); } while ($result != 0);
$result = system("$echoscu -d +tla --profile-bcp195-nd -ic localhost $tlsport >test33_client_out.log 2>test33_client_err.log");
if ($result == 0) { &testPassed(); } else { &testFailed(); }
&doKill($pid);
$tlsport++;

print "Test 34: client uses NULL TLS Profile, server uses BCP 195 Profile: ";
$pid = &startBackgroundProcess("$storescp -d --profile-bcp195 +tls user1_key.pem user1_cert.pem -ic $tlsport >test34_server_out.log 2>test34_server_err.log");
do { $result = system("$tcpclient localhost $tlsport >tcpclient.log"); } while ($result != 0);
$result = system("$echoscu -d +tla --profile-null -ic localhost $tlsport >test34_client_out.log 2>test34_client_err.log");
if (($result != 0) && (&findInFiles("no shared cipher", "test34_server_err.log", "test34_server_out.log"))) { &testPassed(); } else { &testFailed(); }
&doKill($pid);
$tlsport++;

print "Test 35: client and server use NULL TLS Profile: ";
$pid = &startBackgroundProcess("$storescp -d --profile-null +tls user1_key.pem user1_cert.pem -ic $tlsport >test35_server_out.log 2>test35_server_err.log");
do { $result = system("$tcpclient localhost $tlsport >tcpclient.log"); } while ($result != 0);
$result = system("$echoscu -d +tla --profile-null -ic localhost $tlsport >test35_client_out.log 2>test35_client_err.log");
if ($result == 0) { &testPassed(); } else { &testFailed(); }
&doKill($pid);
$tlsport++;

print "Test 36: client lists supported ciphers: ";
$result = system("$echoscu --list-ciphers >test36_client_out.log 2>test36_client_err.log");
if (($result == 0) && (&findInFiles("TLS_DHE_DSS_WITH_CAMELLIA_128_CBC_SHA", "test36_client_err.log", "test36_client_out.log"))) { &testPassed(); } else { &testFailed(); }

print "Test 37: client adds non-recommended ciphersuite: ";
$pid = &startBackgroundProcess("$storescp -d +tls user1_key.pem user1_cert.pem -ic $tlsport >test37_server_out.log 2>test37_server_err.log");
do { $result = system("$tcpclient localhost $tlsport >tcpclient.log"); } while ($result != 0);
$result = system("$echoscu -d +tla --cipher TLS_RSA_WITH_AES_128_GCM_SHA256 -ic localhost $tlsport >test37_client_out.log 2>test37_client_err.log");
if (($result == 0) && (&findInFiles("RFC 7525 recommends that such cipher suites should not be used", "test37_client_err.log", "test37_client_out.log"))) { &testPassed(); } else { &testFailed(); }
&doKill($pid);
$tlsport++;

print "Test 38: client adds NULL ciphersuite: ";
$pid = &startBackgroundProcess("$storescp -d +tls user1_key.pem user1_cert.pem -ic $tlsport >test38_server_out.log 2>test38_server_err.log");
do { $result = system("$tcpclient localhost $tlsport >tcpclient.log"); } while ($result != 0);
$result = system("$echoscu -d +tla --cipher TLS_RSA_WITH_NULL_SHA -ic localhost $tlsport >test38_client_out.log 2>test38_client_err.log");
if (($result != 0) && (&findInFiles("Unencrypted ciphersuite 'TLS_RSA_WITH_NULL_SHA' not permitted", "test38_client_err.log", "test38_client_out.log"))) { &testPassed(); } else { &testFailed(); }
&doKill($pid);
$tlsport++;

print "Test 39: client adds CAMELLIA ciphersuite: ";
$pid = &startBackgroundProcess("$storescp -d +tls user1_key.pem user1_cert.pem -ic $tlsport >test39_server_out.log 2>test39_server_err.log");
do { $result = system("$tcpclient localhost $tlsport >tcpclient.log"); } while ($result != 0);
$result = system("$echoscu -d +tla --cipher TLS_DHE_DSS_WITH_CAMELLIA_128_CBC_SHA -ic localhost $tlsport >test39_client_out.log 2>test39_client_err.log");
if ($result == 0) { &testPassed(); } else { &testFailed(); }
&doKill($pid);
$tlsport++;

print "Tests completed: $numPassed tests passed, $numFailed tests failed.\n";
exit(0);

###############################################################################

sub testPassed
{
  print "passed.\n";
  $numPassed++;
}

sub testFailed
{
  print "failed.\n";
  $numFailed++;
}

#
# startBackgroundProcess(command)
# run command in separate process
#
sub startBackgroundProcess
{
   my  $pid = fork();
   if ($pid)
   { # Parent
      return $pid;
   }
   elsif ($pid == 0)
   { # Child
     if (! $windows)
     {
       setpgrp(0,0); # set process group to process ID; this function is not available on Windows.
     }
     # this allows us to use kill '-TERM' to kill both the shell actually started by exec()
     # and the real process (e.g. storescp) started from the shell, which has a different PID
     # and is a child process of this one, i.e. inherits its process group ID.
     exec(@_) || die("unable to execute background command");
   }
   else
   { # Unable to fork
     die "Error: unable to fork.";
   }
}

#
# find line matching regular expression in a text file
# @param regex regular expression
# @param filename filename
#
sub findInFile
{
  local($regex, $filename) = @_;
  open(INFILE, "<$filename") || die "unable to open file '$filename'";
  while (my $row = <INFILE>)
  {
    chomp $row;
    if ($row =~ /$regex/) { close INFILE; return 1; }
  }
  close INFILE;
  return 0;
}

#
# find line matching regular expression in two text files (for stdout and stderr)
# @param regex regular expression
# @param filename1 filename 1
# @param filename2 filename 2
#
sub findInFiles
{
  local($regex, $filename1, $filename2) = @_;
  open(INFILE, "<$filename1") || die "unable to open file '$filename1'";
  while (my $row = <INFILE>)
  {
    chomp $row;
    if ($row =~ /$regex/) { close INFILE; return 1; }
  }
  close INFILE;
  open(INFILE2, "<$filename2") || die "unable to open file '$filename2'";
  while (my $row = <INFILE2>)
  {
    chomp $row;
    if ($row =~ /$regex/) { close INFILE2; return 1; }
  }
  close INFILE2;
  return 0;
}

#
# compute MD5 digest of a file
# @param filename filename
#
sub md5digest
{
  local($filename) = @_;
  open (my $fh, '<', $filename) || die "unable to open file '$filename'";
  binmode ($fh);
  local ($result) = Digest::MD5->new->addfile($fh)->hexdigest;
  close ($fh);
  return $result;
}

#
# kill process group
# @param pid process ID
#
sub doKill
{
  local($pid) = @_;
  # don't use kill on Windows, because here $pid will always point to the Perl interpreter, i.e. we'd kill ourselves.
  if (! $windows) { kill '-TERM', $pid; }
}
