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
#    This script generates the set of files needed to run the functional
#    (integration) test suite for module dcmtls.
#    This script requires a Linux/Posix system, but the generated files
#    can also be used on other platforms such as Windows.
#

# First we need to generate three certification authorities
&createNewCA("ca1", "DCMTK Test CA 1");
system ("cp ca1/cacert.pem ./cacert1.pem");
&createNewCA("ca2", "DCMTK Test CA 2");
system ("cp ca2/cacert.pem ./cacert2.pem");
&createNewCA("ca3", "DCMTK Test CA 3");
system ("cp ca3/cacert.pem ./cacert3.pem");
mkdir("cadir", 0755);
$cahash1 = `openssl x509 -hash -noout -in ./cacert1.pem`;
chop $cahash1;
$cahash2 = `openssl x509 -hash -noout -in ./cacert2.pem`;
chop $cahash2;

# Generate directory with CA certificates
system ("cp ./cacert1.pem cadir/$cahash1.0");
if ($cahash1 eq $cahash2)
{
  # both certificates have the same hash key. Unlikely but possible.
  system ("cp ./cacert2.pem cadir/$cahash1.1");
} else {
  system ("cp ./cacert2.pem cadir/$cahash2.0");
}

# Now generate user certificates
# user 1 is a normal certificate/key pair
&createNewCertificate("ca1", "user1_cert.pem", "user1_key.pem", "Test User 1", "-nodes", "", 2048);
# this is a certificate/key pair encoded in DER format (instead of PEM).
# We create a PEM certificate and then convert to DER
&createNewCertificate("ca1", "user2_cert.pem", "user2_key.pem", "DER Test User", "-nodes", "", 2048);
system ("openssl x509 -in user2_cert.pem -out user2_cert.der -outform DER");
system ("openssl pkey -in user2_key.pem -out user2_key.der -outform DER");
unlink("user2_cert.pem");
unlink("user2_key.pem");
# user 3 uses an encrypted private key. The password is "PASSWORD".
&createNewCertificate("ca1", "user3_cert.pem", "user3_key.pem", "Test User 3", "-passout pass:PASSWORD", "", 2048);
# user 4 uses a weak 512-bit RSA private key.
&createNewCertificate("ca1", "user4_cert.pem", "user4_key.pem", "Test User 4 (512-bit RSA key)", "-nodes", "", 512);
# user 5 is a certificate that is signed with MD5, a weak hash key
&createNewCertificate("ca1", "user5_cert.pem", "user5_key.pem", "Test User 5 (MD5 digest)", "-nodes", "-md md5", 2048);
# ca2user is a certificate/key pair issued by CA 2
&createNewCertificate("ca2", "ca2user_cert.pem", "ca2user_key.pem", "CA2 Test User 1", "-nodes", "", 2048);
# ca3user is a certificate/key pair issued by CA 3
&createNewCertificate("ca3", "ca3user_cert.pem", "ca3user_key.pem", "CA3 Test User 1", "-nodes", "", 2048);

# Generate other files needed

# A file with 1024 random bits for seeding the PRNG
system ("openssl rand -out randseed.bin 1024");

# A Diffie-Hellman parameter set with 2048 bits
system ("openssl dhparam -out dhparam2048.pem 2048");

# A weak Diffie-Hellman parameter set with only 512 bits
system ("openssl dhparam -out dhparam512.pem 512");

#
# createNewCertificate(string cadir, string certfile, string certkey)
#   creates a new key pair consisting of a private key file and a certificate file
#   signed by the CA.
#   @param ca_directory directory of the CA
#   @param certfile   certificate file to be written
#   @param certkey    private key file to be written
#   @param subject    certificate subject name (CN)
#   @req_options      additional parameters for openssl req
#   @ca_options       additional parameters for openssl ca
#
sub createNewCertificate
{
  local($ca_directory, $certfile, $certkey, $subject, $req_options, $ca_options, $bits) = @_;
  local($days) = 3652;
  if (! -d "$ca_directory")
  {
    die "error: $ca_directory not found, bailing out.";
  }
  system ("openssl req -config ${ca_directory}/openssl.cnf $req_options -subj '/C=DE/ST=Niedersachsen/O=OFFIS/OU=OFFIS DICOM Team/emailAddress=dicom\@offis.de/CN=$subject' -newkey rsa:$bits -keyout $certkey -out ${ca_directory}/private/tempreq.pem");
  system ("openssl ca -config ${ca_directory}/openssl.cnf  $ca_options -policy policy_anything -in ${ca_directory}/private/tempreq.pem -out $certfile -days $days -batch");
  unlink "${ca_directory}/private/tempreq.pem";
  @newfiles = `cd ${ca_directory}/newcerts; ls *.pem`;
  foreach (@newfiles)
  {
    chop;
    system("mv ${ca_directory}/newcerts/$_ ${ca_directory}/certs");
    system("cd ${ca_directory}/certs; ln -s $_ `openssl x509 -hash -noout -in $_`.0");
  }
  return $?;
}


# createNewCA(directory, CA name)
#   creates all files required for a new Certification Authority
#   @param directory  directory to be created for the new CA, must not yet exist.
#
sub createNewCA
{
  local($ca_directory, $ca_name) = @_;
  local($days) = 7305; # 20 years
  local($bits) = 4096; # RSA key length
  if (-e "$ca_directory")
  {
    print "warning: $ca_directory already exists, skipping CA creation step.\n";
    return;
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
  system ("openssl rand -out ${ca_directory}/private/randseed.bin 1024");
  system ("openssl req -config ${ca_directory}/openssl.cnf -newkey rsa:$bits -x509 -subj '/C=DE/ST=Niedersachsen/O=OFFIS/OU=$ca_name/emailAddress=dicom\@offis.de' -nodes -keyout ${ca_directory}/private/cakey.pem -out ${ca_directory}/cacert.pem -days $days");
  system ("cp ${ca_directory}/cacert.pem ${ca_directory}/certs/00.pem");
  system ("cd ${ca_directory}/certs; ln -s 00.pem `openssl x509 -hash -noout -in 00.pem`.0");
  return $?;
}

#
# createNewConfigFile(directory)
#   creates new default configuration file named openssl.cnf in CA directory.
#   @param directory  CA directory.
#
sub createNewConfigFile
{
  local($ca_directory) = @_;
  open OUT, ">${ca_directory}/openssl.cnf";

#####################################################################
print OUT <<END_OF_CONFIGURATION_FILE;
#
# OpenSSL configuration file for DCMTK
# This is mostly being used for generation of certificate requests.
#

# This definition stops the following lines choking if HOME isn't
# defined.
HOME            = .
RANDFILE        = \$ENV::HOME/.rnd

# Extra OBJECT IDENTIFIER info:
#oid_file       = \$ENV::HOME/.oid
oid_section     = new_oids

# To use this configuration file with the "-extfile" option of the
# "openssl x509" utility, name here the section containing the
# X.509v3 extensions to use:
# extensions        =
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
default_ca  = CA_default        # The default ca section

####################################################################
[ CA_default ]

dir     = ${ca_directory}   # Where everything is kept
certs       = \$dir/certs       # Where the issued certs are kept
crl_dir     = \$dir/crl     # Where the issued crl are kept
database    = \$dir/index.txt   # database index file.
new_certs_dir   = \$dir/newcerts    # default place for new certs.

certificate = \$dir/cacert.pem  # The CA certificate
serial      = \$dir/serial      # The current serial number
crl     = \$dir/crl.pem         # The current CRL
private_key = \$dir/private/cakey.pem   # The private key
RANDFILE    = \$dir/private/randseed.bin    # private random number file

x509_extensions = usr_cert      # The extentions to add to the cert

# Extensions to add to a CRL. Note: Netscape communicator chokes on V2 CRLs
# so this is commented out by default to leave a V1 CRL.
# crl_extensions    = crl_ext

default_days    = 3652          # how long to certify for
default_crl_days= 30            # how long before next CRL
default_md  = sha256        # which md to use.
preserve    = no            # keep passed DN ordering
prompt = no

# A few difference way of specifying how similar the request should look
# For type CA, the listed attributes must be the same, and the optional
# and supplied fields are just that :-)
policy      = policy_match

# For the CA policy
[ policy_match ]
countryName     = match
stateOrProvinceName = match
organizationName    = match
organizationalUnitName  = optional
commonName      = supplied
emailAddress        = optional

# For the 'anything' policy
# At this point in time, you must list all acceptable 'object'
# types.
[ policy_anything ]
countryName     = optional
stateOrProvinceName = optional
localityName        = optional
organizationName    = optional
organizationalUnitName  = optional
commonName      = supplied
emailAddress        = optional

####################################################################
[ req ]

default_bits        = 2048
default_keyfile     = privkey.pem
distinguished_name  = req_distinguished_name
attributes      = req_attributes
x509_extensions     = v3_ca # The extentions to add to the self signed cert

# Passwords for private keys if not present they will be prompted for
# input_password = secret
# output_password = secret

# This sets a mask for permitted string types. There are several options.
# default: PrintableString, T61String, BMPString.
# pkix   : PrintableString, BMPString.
# utf8only: only UTF8Strings.
# nombstr : PrintableString, T61String (no BMPStrings or UTF8Strings).
# MASK:XXXX a literal mask value.
# WARNING: current versions of Netscape crash on BMPStrings or UTF8Strings
# so use this option with caution!
string_mask = nombstr

# req_extensions = v3_req # The extensions to add to a certificate request

[ req_distinguished_name ]
countryName         = Country Name (2 letter code)
countryName_default     = DE
countryName_min         = 2
countryName_max         = 2

stateOrProvinceName     = State or Province Name (full name)
stateOrProvinceName_default = Some-State

localityName            = Locality Name (eg, city)

0.organizationName      = Organization Name (eg, company)
0.organizationName_default  = OFFIS

# we can do this but it is not needed normally :-)
#1.organizationName     = Second Organization Name (eg, company)
#1.organizationName_default = World Wide Web Pty Ltd

organizationalUnitName      = Organizational Unit Name (eg, section)
organizationalUnitName_default  = DCMTK

commonName          = Common Name (eg, YOUR name)
commonName_max          = 64

emailAddress            = Email Address
emailAddress_max        = 40
emailAddress_default            = dicom\@offis.de

# SET-ex3           = SET extension number 3

[ req_attributes ]
challengePassword       = A challenge password
challengePassword_min       = 4
challengePassword_max       = 20

unstructuredName        = An optional company name

[ usr_cert ]

# These extensions are added when 'ca' signs a request.

# This goes against PKIX guidelines but some CAs do it and some software
# requires this to avoid interpreting an end user certificate as a CA.

basicConstraints=CA:FALSE

# Here are some examples of the usage of nsCertType. If it is omitted
# the certificate can be used for anything *except* object signing.

# This is OK for an SSL server.
# nsCertType            = server

# For an object signing certificate this would be used.
# nsCertType = objsign

# For normal client use this is typical
# nsCertType = client, email

# and for everything including object signing:
# nsCertType = client, email, objsign

# This is typical in keyUsage for a client certificate.
# keyUsage = nonRepudiation, digitalSignature, keyEncipherment

# This will be displayed in Netscape's comment listbox.
nsComment           = "OpenSSL Generated Certificate"

# PKIX recommendations harmless if included in all certificates.
subjectKeyIdentifier=hash
authorityKeyIdentifier=keyid,issuer:always

# This stuff is for subjectAltName and issuerAltname.
# Import the email address.
# subjectAltName=email:copy

# Copy subject details
# issuerAltName=issuer:copy

#nsCaRevocationUrl      = http://www.domain.dom/ca-crl.pem
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
