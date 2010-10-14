#!/usr/local/bin/perl
#
#  Copyright (C) 1998-2010, OFFIS e.V.
#  All rights reserved.  See COPYRIGHT file for details.
#
#  This software and supporting documentation were developed by
#
#   OFFIS e.V.
#   R&D Division Health
#   Escherweg 2
#   D-26121 Oldenburg, Germany
#
#
#  Module: dcmpstat
#
#  Author: Marco Eichelberg
#
#  Purpose:
#    This script simplifies the set-up and operation of a Certification
#    Authority by means of OpenSSL, for use with the TLS-enhanced
#    DCMTK DICOM Tools.
#
#  Last Update:      $Author: joergr $
#  Update Date:      $Date: 2010-10-14 13:02:47 $
#  CVS/RCS Revision: $Revision: 1.4 $
#  Status:           $State: Exp $
#
#  CVS/RCS Log at end of file
#

$const_openssl="openssl";
$const_ca_key="cakey.pem";
$const_ca_cert="cacert.pem";
$const_ca_config="openssl.cnf";
$const_ca_seed="randseed.bin";

$numCommands = 0;
for ($i=0; $i<=$#ARGV; $i++)
{
  $_ = $ARGV[$i];
  if (/^-/)
  {
    $options{$_} = $ARGV[++$i];
  } else {
    $command[$numCommands++] = $_;
  }
}

if ($numCommands < 1) { &usageAndExit(); }

if ($command[0] eq 'newca')
{
  if ($numCommands != 2)
  {
    &usageAndExit();
  }
  &createNewCA($command[1]);
  exit($?);
}
elsif ($command[0] eq 'mkcert')
{
  if ($numCommands != 4)
  {
    &usageAndExit();
  }
  &createNewCertificate($command[1], $command[2], $command[3]);
  exit($?);
} else {
  &usageAndExit();
}
exit(0);

#
# usageAndExit()
#  print usage string and terminate.
#
sub usageAndExit
{
print <<eof;
usage: dcmtk_ca.pl command [options] arguments
  known commands are:
    newca [options] directory
      generate a new Certification Authority; directory must not yet exist.
      -type rsa|dsa                      type of CA certificate (default:rsa)
      -days days                         validity of CA certificate (365)
      -bits bits                         bits in CA key (default: 2048)
    mkcert [options] ca_dir certfile keyfile
      generate a new key pair and use the Certification Authority in ca_dir
      to certify the public key; write certificate to certfile and the private
      key to keyfile.
      -type rsa|dsa                      type of certificate (default:rsa)
      -days days                         validity of certificate (365)
      -bits bits                         bits in private key (default: 1024)
      -des yes|no                        encrypt private key (default: yes)
      -pkcs12 filename                   export certificate to PKCS#12
      -pkcs12name name                   PKCS#12 symbolic name
eof
  exit(0);
}

#
# createNewCA(string directory)
#   creates all files required for a new Certification Authority
#   @param directory  directory to be created for the new CA, must not yet exist.
#
sub createNewCA
{
  local($ca_directory) = @_;
  local($days) = $options{'-days'};
  if ($days ne '') { $days = "-days $days"; }
  local($bits) = $options{'-bits'};
  if ($bits == 0) { $bits = 2048; }
  local($type) = $options{'-type'};
  local($keytype);
  if ($type eq 'dsa')
  {
    $keytype = "-newkey dsa:${ca_directory}/private/dsaparam.pem";
  } else {
    $keytype = "-newkey rsa:$bits";
  }
  if (-e "$ca_directory")
  {
    die "error: $ca_directory already exists, bailing out.";
  }
  mkdir($ca_directory, 0700);
  mkdir("${ca_directory}/certs", 0700);
  mkdir("${ca_directory}/crl", 0700);
  mkdir("${ca_directory}/newcerts", 0700);
  mkdir("${ca_directory}/private", 0700);
  open OUT, ">${ca_directory}/serial";
  print OUT "01\n";
  close OUT;
  open OUT, ">${ca_directory}/index.txt";
  close OUT;
  &createNewConfigFile($ca_directory);
  system ("$const_openssl rand -out ${ca_directory}/private/$const_ca_seed 1024");
  if ($type eq 'dsa')
  {
    system ("$const_openssl dsaparam -out ${ca_directory}/private/dsaparam.pem $bits");
  } else {
  }
  system ("$const_openssl req -config ${ca_directory}/$const_ca_config $keytype -x509 -keyout ${ca_directory}/private/$const_ca_key -out ${ca_directory}/$const_ca_cert $days");
  system ("cp ${ca_directory}/$const_ca_cert ${ca_directory}/certs/00.pem");
  system ("cd ${ca_directory}/certs; ln -s 00.pem `openssl x509 -hash -noout -in 00.pem`.0");
  return $?;
}


#
# createNewCertificate(string cadir, string certfile, string certkey)
#   creates a new key pair consisting of a private key file and a certificate file
#   signed by the CA.
#   @param cadir  directory of the CA
#   @param certfile   certificate file to be written
#   @param certkey    private key file to be written
#
sub createNewCertificate
{
  local($ca_directory, $certfile, $certkey) = @_;
  local($days) = $options{'-days'};
  if ($days ne '') { $days = "-days $days"; }
  local($ca_certificate) = $options{'-cacert'};
  local($bits) = $options{'-bits'};
  if ($bits == 0) { $bits = 1024; }
  local($type) = $options{'-type'};
  local($encryption);
  local($des) = $options{'-des'};
  local($pkcs12) = $options{'-pkcs12'};
  local($pkcs12name) = $options{'-pkcs12name'};
  if ($pkcs12name eq '') { $pkcs12name = "OpenSSL generated DCMTK Certificate"; }

  if ($des eq 'no')
  {
    $encryption = '-nodes';
  }
  local($keytype);
  if ($type eq 'dsa')
  {
    $keytype = "-newkey dsa:${ca_directory}/private/tempdh.pem";
  } else {
    $keytype = "-newkey rsa:$bits";
  }
  if (! -d "$ca_directory")
  {
    die "error: $ca_directory not found, bailing out.";
  }
  if ($type eq 'dsa')
  {
    system ("$const_openssl dsaparam -out ${ca_directory}/private/tempdh.pem $bits");
  }
  system ("$const_openssl req -config ${ca_directory}/$const_ca_config $encryption $keytype -keyout $certkey -out ${ca_directory}/private/tempreq.pem");
  system ("$const_openssl ca -config ${ca_directory}/$const_ca_config -policy policy_anything -in ${ca_directory}/private/tempreq.pem -out $certfile $days");
  if ($type eq 'dsa')
  {
    unlink "${ca_directory}/private/tempdh.pem";
  }
  unlink "${ca_directory}/private/tempreq.pem";
  @newfiles = `cd ${ca_directory}/newcerts; ls *.pem`;
  foreach (@newfiles)
  {
    chop;
    system("mv ${ca_directory}/newcerts/$_ ${ca_directory}/certs");
    system("cd ${ca_directory}/certs; ln -s $_ `$const_openssl x509 -hash -noout -in $_`.0");
  }
  if ($pkcs12 ne '')
  {
    system ("$const_openssl pkcs12 -in $certfile -inkey $certkey -certfile ${ca_directory}/$const_ca_cert -out $pkcs12 -export -name \"$pkcs12name\"");
  }
  return $?;
}


#
# createNewConfigFile(string directory)
#   creates new default configuration file named $const_ca_config in CA directory.
#   @param directory  CA directory.
#
sub createNewConfigFile
{
  local($ca_directory) = @_;
  open OUT, ">${ca_directory}/$const_ca_config";

#####################################################################
print OUT <<END_OF_CONFIGURATION_FILE;
#
# OpenSSL configuration file for DCMTK
# This is mostly being used for generation of certificate requests.
#

# This definition stops the following lines choking if HOME isn't
# defined.
HOME			= .
RANDFILE		= \$ENV::HOME/.rnd

# Extra OBJECT IDENTIFIER info:
#oid_file		= \$ENV::HOME/.oid
oid_section		= new_oids

# To use this configuration file with the "-extfile" option of the
# "openssl x509" utility, name here the section containing the
# X.509v3 extensions to use:
# extensions		=
# (Alternatively, use a configuration file that has only
# X.509v3 extensions in its main [= default] section.)

[ new_oids ]

# We can add new OIDs in here for use by 'ca' and 'req'.
# Add a simple OID like this:
# testoid1=1.2.3.4
# Or use config file substitution like this:
# testoid2=\${testoid1}.5.6

####################################################################
[ ca ]
default_ca	= CA_default		# The default ca section

####################################################################
[ CA_default ]

dir		= ${ca_directory}	# Where everything is kept
certs		= \$dir/certs		# Where the issued certs are kept
crl_dir		= \$dir/crl		# Where the issued crl are kept
database	= \$dir/index.txt	# database index file.
new_certs_dir	= \$dir/newcerts		# default place for new certs.

certificate	= \$dir/cacert.pem 	# The CA certificate
serial		= \$dir/serial 		# The current serial number
crl		= \$dir/crl.pem 		# The current CRL
private_key	= \$dir/private/cakey.pem	# The private key
RANDFILE	= \$dir/private/randseed.bin	# private random number file

x509_extensions	= usr_cert		# The extentions to add to the cert

# Extensions to add to a CRL. Note: Netscape communicator chokes on V2 CRLs
# so this is commented out by default to leave a V1 CRL.
# crl_extensions	= crl_ext

default_days	= 365			# how long to certify for
default_crl_days= 30			# how long before next CRL
default_md	= md5			# which md to use.
preserve	= no			# keep passed DN ordering

# A few difference way of specifying how similar the request should look
# For type CA, the listed attributes must be the same, and the optional
# and supplied fields are just that :-)
policy		= policy_match

# For the CA policy
[ policy_match ]
countryName		= match
stateOrProvinceName	= match
organizationName	= match
organizationalUnitName	= optional
commonName		= supplied
emailAddress		= optional

# For the 'anything' policy
# At this point in time, you must list all acceptable 'object'
# types.
[ policy_anything ]
countryName		= optional
stateOrProvinceName	= optional
localityName		= optional
organizationName	= optional
organizationalUnitName	= optional
commonName		= supplied
emailAddress		= optional

####################################################################
[ req ]

default_bits		= 1024
default_keyfile 	= privkey.pem
distinguished_name	= req_distinguished_name
attributes		= req_attributes
x509_extensions	= v3_ca	# The extentions to add to the self signed cert

# Passwords for private keys if not present they will be prompted for
# input_password = secret
# output_password = secret

# This sets a mask for permitted string types. There are several options.
# default: PrintableString, T61String, BMPString.
# pkix	 : PrintableString, BMPString.
# utf8only: only UTF8Strings.
# nombstr : PrintableString, T61String (no BMPStrings or UTF8Strings).
# MASK:XXXX a literal mask value.
# WARNING: current versions of Netscape crash on BMPStrings or UTF8Strings
# so use this option with caution!
string_mask = nombstr

# req_extensions = v3_req # The extensions to add to a certificate request

[ req_distinguished_name ]
countryName			= Country Name (2 letter code)
countryName_default		= DE
countryName_min			= 2
countryName_max			= 2

stateOrProvinceName		= State or Province Name (full name)
stateOrProvinceName_default	= Some-State

localityName			= Locality Name (eg, city)

0.organizationName		= Organization Name (eg, company)
0.organizationName_default	= OFFIS

# we can do this but it is not needed normally :-)
#1.organizationName		= Second Organization Name (eg, company)
#1.organizationName_default	= World Wide Web Pty Ltd

organizationalUnitName		= Organizational Unit Name (eg, section)
organizationalUnitName_default	= DCMTK

commonName			= Common Name (eg, YOUR name)
commonName_max			= 64

emailAddress			= Email Address
emailAddress_max		= 40
emailAddress_default	        = dicom\@offis.de

# SET-ex3			= SET extension number 3

[ req_attributes ]
challengePassword		= A challenge password
challengePassword_min		= 4
challengePassword_max		= 20

unstructuredName		= An optional company name

[ usr_cert ]

# These extensions are added when 'ca' signs a request.

# This goes against PKIX guidelines but some CAs do it and some software
# requires this to avoid interpreting an end user certificate as a CA.

basicConstraints=CA:FALSE

# Here are some examples of the usage of nsCertType. If it is omitted
# the certificate can be used for anything *except* object signing.

# This is OK for an SSL server.
# nsCertType			= server

# For an object signing certificate this would be used.
# nsCertType = objsign

# For normal client use this is typical
# nsCertType = client, email

# and for everything including object signing:
# nsCertType = client, email, objsign

# This is typical in keyUsage for a client certificate.
# keyUsage = nonRepudiation, digitalSignature, keyEncipherment

# This will be displayed in Netscape's comment listbox.
nsComment			= "OpenSSL Generated Certificate"

# PKIX recommendations harmless if included in all certificates.
subjectKeyIdentifier=hash
authorityKeyIdentifier=keyid,issuer:always

# This stuff is for subjectAltName and issuerAltname.
# Import the email address.
# subjectAltName=email:copy

# Copy subject details
# issuerAltName=issuer:copy

#nsCaRevocationUrl		= http://www.domain.dom/ca-crl.pem
#nsBaseUrl
#nsRevocationUrl
#nsRenewalUrl
#nsCaPolicyUrl
#nsSslServerName

# Extensions to add to a certificate request
[ v3_req ]

basicConstraints = CA:FALSE
keyUsage = nonRepudiation, digitalSignature, keyEncipherment


# Extensions for a typical CA
[ v3_ca ]

# PKIX recommendation.
subjectKeyIdentifier=hash

authorityKeyIdentifier=keyid:always,issuer:always

# This is what PKIX recommends but some broken software chokes on critical
# extensions.
#basicConstraints = critical,CA:true
# So we do this instead.
basicConstraints = CA:true

# Key usage: this is typical for a CA certificate. However since it will
# prevent it being used as an test self-signed certificate it is best
# left out by default.
keyUsage = cRLSign, keyCertSign

# Some might want this also
nsCertType = sslCA, emailCA, objCA

# Include email address in subject alt name: another PKIX recommendation
subjectAltName         = email:copy
# Copy issuer details
issuerAltName          = issuer:copy

# crlDistributionPoints  = URI:http://crlserver.domain.de/CA.crl

[ crl_ext ]

# CRL extensions.
# Only issuerAltName and authorityKeyIdentifier make any sense in a CRL.

issuerAltName=issuer:copy
authorityKeyIdentifier=keyid:always,issuer:always

END_OF_CONFIGURATION_FILE
#####################################################################

  close OUT;
}


#
#  $Log: dcmtk_ca.pl,v $
#  Revision 1.4  2010-10-14 13:02:47  joergr
#  Updated copyright header. Added reference to COPYRIGHT file.
#
#  Revision 1.3  2001/06/01 16:51:53  meichel
#  Fixed bug in CA perl script. mkcert -days option now works.
#
#  Revision 1.2  2001/06/01 15:51:14  meichel
#  Updated copyright header
#
#  Revision 1.1  2000/11/14 13:36:33  meichel
#  Added Perl script that simplifies creation of a Certification Authority,
#    private keys and X.509 certificates with OpenSSL for use with DCMTK.
#
#
