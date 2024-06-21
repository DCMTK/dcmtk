/*
 *
 *  Copyright (C) 2019-2023, OFFIS e.V.
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  This software and supporting documentation were developed by
 *
 *    OFFIS e.V.
 *    R&D Division Health
 *    Escherweg 2
 *    D-26121 Oldenburg, Germany
 *
 *
 *  Module:  dcmtls
 *
 *  Authors:  Michel Amat, Damien Lerat
 *
 *  Purpose: TLS test for classes DcmSCP and DcmSCPPool
 *
 *  Note: This test will fail after 2029-02-25 due to certificate expiry.
 *        The keys embedded in this file should be replaced then (see below).
 *
 */

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/ofstd/oftest.h"
#include "dcmtk/ofstd/oftimer.h"
#include "dcmtk/ofstd/ofrand.h"
#include "dcmtk/oflog/consap.h"
#include "dcmtk/dcmnet/scp.h"
#include "dcmtk/dcmnet/scu.h"
#include "dcmtk/dcmnet/scppool.h"
#include "dcmtk/dcmnet/dcmlayer.h"
#include "dcmtk/dcmtls/tlsscu.h"
#include <cmath>

#ifdef WITH_THREADS
#ifdef WITH_OPENSSL

#define BAILOUT(msg) do { \
    OFCHECK_FAIL(msg); \
    return; \
} while (0)

/** Method that ensures that the current thread is actually sleeping for the
 *  defined number of seconds (at least).
 *  The problem with the regular sleep() function called from OFStandard::sleep
 *  is that it might be interrupted by signals or a network timeout (depending
 *  on the operating system). This methods re-executes OFStandard's sleep method
 *  until the desired number of seconds have elapsed.
 *  @param sleep The number of seconds to sleep (at least)
 */
static void force_sleep(Uint32 sleep)
{
    OFTimer timer;
    double elapsed = timer.getDiff();
    while (elapsed < (double)sleep)
    {
        // Use ceiling since otherwise we could wait too short
        OFStandard::sleep(OFstatic_cast(unsigned int, ceil(sleep-elapsed)));
        elapsed = timer.getDiff();
    }
}


/** TestSCP derived from DcmSCP in order to test TLS functionality
 *  through function setTransportLayer of DcmSCPConfig.
 *
 *  Additionally the SCP derives from OFThread in order to construct the
 *  test case (i.e. send data with SCU at the same time and check results).
 */
struct TestSCP: DcmSCP, OFThread
{
    /** Constructor
     */
    TestSCP() :
        DcmSCP(),
        m_listen_result(EC_NotYetImplemented), // value indicating "not set"
        m_set_stop_after_assoc(OFFalse),
        m_set_stop_after_timeout(OFFalse),
        m_is_running(OFFalse)
    {
    }

    /** Clear settings
     */
    void clear()
    {
        m_listen_result = EC_NotYetImplemented;
        m_set_stop_after_assoc = OFFalse;
        m_set_stop_after_timeout = OFFalse;
        m_is_running = OFFalse;
    }

    /** Overwrite method from DcmSCP in order to test feature to stop after current
     *  association.
     *  @return Returns value of m_set_stop_after_assoc
     */
    virtual OFBool stopAfterCurrentAssociation()
    {
        return m_set_stop_after_assoc;
    }

    /** Overwrite method from DcmSCP in order to test feature to stop after
    *   the SCP's connection timeout occurred in non-blocking mode.
    *   @return Returns value of m_set_stop_after_timeout
    */
    virtual OFBool stopAfterConnectionTimeout()
    {
        return m_set_stop_after_timeout;
    }

    /// The result returned my SCP's listen() method
    OFCondition m_listen_result;
    /// If set, the SCP should stop after the currently running association
    OFBool m_set_stop_after_assoc;
    /// If set, the SCP should stop after TCP timeout occurred in non-blocking mode
    OFBool m_set_stop_after_timeout;
    /// indicates whether the thread is currently running
    volatile OFBool m_is_running;

    /** Method called by OFThread to start SCP operation. Starts listen() loop of DcmSCP.
    */
    virtual void run()
    {
        m_is_running = OFTrue;
        m_listen_result = listen();
        m_is_running = OFFalse;
    }

};

// -------------- End of class TestSCP -------------------------

struct TestPool : DcmSCPPool<>, OFThread
{
    OFCondition m_listen_result;
    volatile OFBool m_is_running;

    TestPool()
    : DcmSCPPool<>()
    , OFThread()
    , m_listen_result(EC_NotYetImplemented)
    , m_is_running(OFFalse)
    { }

protected:
    void run()
    {
        m_is_running = OFTrue;
        m_listen_result = listen();
        m_is_running = OFFalse;
    }
};

// -------------- End of class TestPool -------------------------

struct TestTLSSCU : DcmTLSSCU, OFThread
{
    OFCondition result;
protected:
    void run()
    {
        OFCHECK(negotiateAssociation().good());
        result = sendECHORequest(0);
        releaseAssociation();
    }
};

// -------------- End of class TestTLSSCU -------------------------


#define PRIVATE_KEY_FILENAME "privkey.pem"
#define PUBLIC_SELFSIGNED_CERT_FILENAME "pubselfsignedcert.pem"
#define PRIVATE_KEY_PWD "testDcmtk!"


/** Method write_temp_key_cert_files writes private key and public certificate files,
 *  needed by tests dcmtls_scp_tls and dcmtls_scp_pool_tls.
 *  It corresponds to the files generated by the following openssl command>
 *  > openssl req -x509 -newkey rsa:4096 -keyout PRIVATE_KEY_FILENAME  -passout pass:PRIVATE_KEY_PWD
 *    -out PUBLIC_SELFSIGNED_CERT_FILENAME -days 3653 -subj "/C=DE/ST=DcmTls/L=Tests/O=OFFIS/OU=DCMTK/CN=dicom@offis.de"
 *  @note The certificate will expire on 2029-02-25. The keys should be replaced then.
 */
void write_temp_key_cert_files()
{
    const char * privKey="-----BEGIN ENCRYPTED PRIVATE KEY-----\n"
"MIIJnDBOBgkqhkiG9w0BBQ0wQTApBgkqhkiG9w0BBQwwHAQIfH2W1N63B6wCAggA\n"
"MAwGCCqGSIb3DQIJBQAwFAYIKoZIhvcNAwcECPvpj/Xqx43SBIIJSHU3VPzHDKNX\n"
"8c7vnHT1OYjeBkx6EsnzewuWIhnzDFdt1SOXgIGUZrXVcxln1t0TvyZug+zEPY7L\n"
"TxQy/pT3N81coXzFF+Pj67wOVSVOZNtoEJIPzDvKYzTHhtX40TUK8R1rLyArMD6P\n"
"cODkY+pBUOhd9BlRr1tkaMv0GTAjxv+OhJuI/hothmRtyiZelTitxb4eHRD9ycbP\n"
"bGu3nmTf/MgcPohL/PUV9PIruRGMImQN8XDrP6Ekkp5HWi4OBkZcj37fM6N6/RZ7\n"
"fB8Mc1ebk7sRlssSoiw5LNpAz8M+geUjwTljU3rGTS/KnCMVmvcsnbMahqIrTzub\n"
"I3SPfxHKm5WjeBXDlTBONMGvEzKKXYoAV6XWKkvLClhkp+oQweOAzzHk/SJJYU7s\n"
"wmnfGGrkG26HDE0sYP5BTFI6wipnWXcLmXVVN8cawqh74Tfc3HZR0KNEiIb0nox+\n"
"zWsXYHf7OVEyj72R+Vho77PcKHDsyYNdm++VwZlESbvzZkwyZRqzrzx7o93cODK+\n"
"GIrvOIHVJ6qW3U/mkLh4RG+XjZimJ1ksK3HIwQirtDrOuvHi+GaBSgp7R5niOkA+\n"
"0PIJ4FzuBvRyXLPmSZlgo5eaGs+JrdYQSLgmiXAEmlm/200u3xtW3la6FhZyR6Mc\n"
"moNdW4B+G/0Ra4KFVv/2GCT2vGHT+5bbnpgeviMxn3/k6EtYuoFS9lcHzHrKppwj\n"
"Xx9ssbSR2SgoBSuxM4/69KzkIb0Ow7Sxex7lFt7q01Qtr26Iz4lioiaozMmow362\n"
"4XQsYck8eLakC5Ygmh5hCUpD0GtdY3n+8DT9OTfUFAE8k3PHjNRTI2SPs4F4Nlbf\n"
"WSOmBnQHb3/TD39VaqLhmJMAdax3izSnjk3ePg1JKhVJnGo6saYa8iYNZgbRn1/2\n"
"Xk4++UZVGaiorRHYpXx8UcNSdGkJpE6wqki5zbUhk0YtvzYC54DA260G0aZW0BEp\n"
"PTyhLnPW4OZWJwG3rwYC11DYees030WA0gCfwess3MP+fmZQfGMIZioAG2WVMgIl\n"
"yys4HaPb5vQkjTRqTefFEvfAXxKFfFgrnWwlyzsB9A5mglQYyCl7wTQt9FiF4s5q\n"
"SODDD2Nk72RYJc+JnaARb97OWHUFcmF5MoXJdokEKm2pYa3fcJD1UaBoquufedZY\n"
"uyjnBXz7DxPV8ZhI9KYAX+3BIo6DedPUb+43WcgNXnyqI5xnX/TWeFt4kCp60WFa\n"
"Yku3Og7QIy/PTfwXNvG6yP9koOZqANwdroN8bwwS2y3f3oTkhsy0YgcQWlIPGl6d\n"
"/trbWC2EkNBQOMsE0tRxRnZcP3OOw4pHAPqv71EFUHp9gLxvGb9rMuKqcVEC53xJ\n"
"omFrlV1x8Pa4GO3t0xg1Jatx4Sly9FOi8KqfFGUiyN1NdPuBw4+gpC9F7i5RpBjW\n"
"Yrl/mqZizGNtzRNLAESGoVD/LANmVL9SKNPp9tPDsVIWtVcCBfJVietBqq1JyiKX\n"
"6hdMNaXnAPXv1G95kczGkOjFDIs2lII9ChkY4I6wes0K6n4WbC5kuf3Z/ZAedTdI\n"
"kpQxbFYi+B/95foQM5MrHXibn+SOAO0DzqfOdG0ymBn0X/oeXbgxENmF/fZqE6Ab\n"
"MdusaIy3mnut8yQI3t1Tv/Ei2WHmnXM2bTkyd5OGZK5wYYZlMDXVPBXlZ5ZgRDRv\n"
"zE8hDYp5sXjjDvzVJjMWPj80qcdjY9wvNaoCXpdVHh4zZVgnO9L/r1XrpkIxg9/E\n"
"H3sihqtpTBs6CIDoMGiKI7dkpRyyILLhxggnpf/OEbUTypaobnZ/sOMEmNw8DGR/\n"
"/2c6ECr2/j283vifz1tiEV29Y/Pb1emiW0+66Cjoge2DpHmdB+d2va98OQzaT8Z2\n"
"hBLpIZtELrSZ5DLaVQG/aK5ZS9FxwaWD/Jrj34HJ16cMMSLXH/1eg89XqUaItQ40\n"
"eGWEWdL7vhdNnhkvUqubOc6y4US3EMGm0oB+72KhlvUwrahI/lzWGlevVjstbVAb\n"
"WS/Tg5Jf5/tObUe8OxiW4+NXn/4sx5+SZ3uQRqnLOBFNWSgwE/32S4yCZQjRJZrd\n"
"aNVZybpk5iivH2nKpaA0e0iCTCuqB4ssERspyVeMlnlJ7GSYErq459dkMRWA/JtP\n"
"C9LM1iBwcZ+RgG0Rot1xWmrsjbf8DbPhCj1GqLOBJ82ehw6uKVrE2njr03aRe3yl\n"
"4oOmtrU0TwqPOW7B/8wrwLd9s/1cIxVt7YTzTuiS7f6PL6ec/d0txyCqotKhjWs3\n"
"arWskJoUoY5A1+yD8RU5QpG/gDuJ+ToPV+GXvds+aXMtTrQKtlkXUWfkd+Z2RxEm\n"
"ZGw8+pFmm0YW80KGu6qN37F5G2MMlxtpiEIAIBfpEmWZxRMFMdbiQtJuK3o3ApXJ\n"
"GcXq6qXGXX+u6mBY7VLS1U4Q9PKJJ0/v1PW+0EHA5eeq1J+ZBGhCeM8j9OZzEwRu\n"
"r66jpdxccd4goh39jyZVEQAPxuDxPXzaDiar7Bxpb5ndef1Ml0SX5fPwZq1k8btH\n"
"udKrUGptJwinQ+F6tkr2LOJlssJ8dyw+rVrTqV1mP4KMwM/2EOSCfs15gBy7fSg2\n"
"srybRUgX87hY2KhzBU2+J7VDbWiwFnZ30y05XiVXDx+4KZUlXIWSAUjHBGTTysu0\n"
"jxEN96vkEWbxzcXCZpg1Q4L2VFfeDurL7D6wbEetwW+5hoiB1k/STyI2MFNxVBUb\n"
"JSTj/KA3Ki3V9wd+YZrOH7HHn1jTzn8QnoP5xvB/uJ9Sn1AWtmawLchg9D3eVSEX\n"
"ggQFSbylGhe+aEHnZwUjDtfvjY++CcurnMSLG2pyHxjzeevhtZg8TfJ7AMHdejBA\n"
"PYnQARJ8Pig7m/D5KNzGaP4i0lQzUMs0+e6UIX82n56Qugy7Wrw2+OV0njG1iDzQ\n"
"lXaYEFs4DPk4ATPrxFZlNpOW+VjAwU+GrKvYkRu2kYwAz3AdsbBv1Gq3o0vx5EEM\n"
"36Z13eCtP1N35M9oSFtfh2q7Yu0+xqNZ6Iy4vuxiARp4T7nzv3Q39O+0rC+YDGw3\n"
"Eh5nzkv/KOALmJSPv2ZLZ/lPH+5DW8Khg9YOJpB1q0CDQCuxBGdY0Sh54G6xPNfk\n"
"DbepXf+ja9mUcs6u07JXLeSgJjdkwu5Grj2Bbszfn7fL2fjoZgxTU2fUG5DcWDDV\n"
"hVZ9bZ9R94uD1HuG7IB7SQ==\n"
"-----END ENCRYPTED PRIVATE KEY-----";
    const size_t lenPrivKey = 3413;
    FILE *privKeyFile = fopen(PRIVATE_KEY_FILENAME, "wb");
    OFCHECK(privKeyFile);
    OFCHECK(lenPrivKey == fwrite(privKey, 1, lenPrivKey, privKeyFile));
    fclose(privKeyFile);

    const char* pubSelfSIgnedCert =
    "-----BEGIN CERTIFICATE-----\n"
"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\n"
"x+yqEeQzzYQ=\n"
"-----END CERTIFICATE-----";
    const size_t lenPubSelfSignedCert = 2016;
    FILE *pubSelfSignedCertFile = fopen(PUBLIC_SELFSIGNED_CERT_FILENAME, "wb");
    OFCHECK(pubSelfSignedCertFile);
    OFCHECK(lenPubSelfSignedCert == fwrite(pubSelfSIgnedCert, 1, lenPubSelfSignedCert, pubSelfSignedCertFile));
    fclose(pubSelfSignedCertFile);
}

// Function for initialization of logs with console appender
void initLogs()
{
    const char * const LOG_PATTERN_LAYOUT = "%D{%Y-%m-%d %H:%M:%S.%q} %5p: %m%n";
    OFunique_ptr < dcmtk::log4cplus::Layout > layout(new dcmtk::log4cplus::PatternLayout(LOG_PATTERN_LAYOUT));
    dcmtk::log4cplus::SharedAppenderPtr console(new dcmtk::log4cplus::ConsoleAppender(OFTrue /* logToStdErr */, OFTrue /* immediateFlush */));
    console->setLayout(OFmove(layout));
    dcmtk::log4cplus::Logger log = dcmtk::log4cplus::Logger::getRoot();
    log.removeAllAppenders();
    log.addAppender(console);
    log.setLogLevel(OFLogger::DEBUG_LOG_LEVEL);
}


// Test case that checks server scp tls connection
OFTEST_FLAGS(dcmtls_scp_tls, EF_None)
{
    /// Write key and cert files
    write_temp_key_cert_files();

    /// Init logs
    initLogs();

    /// Init scp tls layer
    OFCondition result;
    DcmTLSTransportLayer scpTlsLayer(NET_ACCEPTOR, NULL, OFTrue);
    scpTlsLayer.setPrivateKeyPasswd(PRIVATE_KEY_PWD);
    result = scpTlsLayer.setPrivateKeyFile(PRIVATE_KEY_FILENAME, DCF_Filetype_PEM);
    OFCHECK(result.good());
    result = scpTlsLayer.setCertificateFile(PUBLIC_SELFSIGNED_CERT_FILENAME, DCF_Filetype_PEM, TSP_Profile_BCP_195_RFC_8996);
    OFCHECK(result.good());
    OFCHECK(scpTlsLayer.checkPrivateKeyMatchesCertificate());
    scpTlsLayer.setCertificateVerification(DCV_ignoreCertificate);

    /// Init and run Scp server with tls
    OFRandom rnd;
    TestSCP scp;
    DcmSCPConfig& config = scp.getConfig();
    config.setAETitle("ACCEPTOR");
    config.setACSETimeout(30);
    config.setConnectionTimeout(1);
    config.setMaxReceivePDULength(16856);
    config.setHostLookupEnabled(false);
    config.setConnectionBlockingMode(DUL_NOBLOCK);
    config.setRespondWithCalledAETitle("REQUESTOR");
    OFList<OFString> xfers;
    xfers.push_back(UID_LittleEndianImplicitTransferSyntax);
    OFCHECK(config.addPresentationContext(UID_VerificationSOPClass, xfers, ASC_SC_ROLE_SCP).good());

    config.setTransportLayer(&scpTlsLayer);

    // Ensure server is up and listening
    int i = 0;
    Uint16 port_number = 0;
    OFMutex memory_barrier;
    do
    {
      // generate a random port number between 61440 (0xF000) and 65535
      port_number = 0xF000 + (rnd.getRND16() & 0xFFF);
      config.setPort(port_number);
      scp.start();
      force_sleep(2); // wait 2 seconds for the SCP process to start
      memory_barrier.lock();
      memory_barrier.unlock();
    }
    while ((i++ < 5) && (! scp.m_is_running)); // try up to 5 port numbers before giving up

    if (! scp.m_is_running) BAILOUT("Start of the SCP thread failed: " << scp.m_listen_result.text());

    // Configure SCU and run it against SCP
    DcmTLSSCU scu;
    scu.setMaxReceivePDULength(16856);
    scu.setDIMSEBlockingMode(DIMSE_NONBLOCKING);
    scu.setDIMSETimeout(300);
    scu.setACSETimeout(30);
    scu.setPeerAETitle("ACCEPTOR");
    scu.setAETitle("REQUESTOR");
    scu.setPeerHostName("localhost");
    scu.setPeerPort(port_number);

    scu.enableAuthentication(PRIVATE_KEY_FILENAME, PUBLIC_SELFSIGNED_CERT_FILENAME, PRIVATE_KEY_PWD, DCF_Filetype_PEM, DCF_Filetype_PEM);
    scu.setPeerCertVerification(DCV_ignoreCertificate);

    OFCHECK(scu.addPresentationContext(UID_VerificationSOPClass, xfers, ASC_SC_ROLE_SCU).good());

    OFCHECK(scu.initNetwork().good());
    OFCHECK(scu.negotiateAssociation().good());

    scp.m_set_stop_after_assoc = OFTrue;
    scp.m_set_stop_after_timeout = OFTrue;
    if (scu.isConnected())
        OFCHECK(scu.releaseAssociation().good());
    scp.join();
}


// Test case that checks server scp tls connection
OFTEST_FLAGS(dcmtls_scp_pool_tls, EF_None)
{
    /// Write key and cert files
    write_temp_key_cert_files();

    /// Init logs
    initLogs();

    /// Init scp tls layer
    OFCondition result;
    DcmTLSTransportLayer scpTlsLayer(NET_ACCEPTOR, NULL, OFTrue);
    scpTlsLayer.setPrivateKeyPasswd(PRIVATE_KEY_PWD);
    result = scpTlsLayer.setPrivateKeyFile(PRIVATE_KEY_FILENAME, DCF_Filetype_PEM);
    OFCHECK(result.good());
    result = scpTlsLayer.setCertificateFile(PUBLIC_SELFSIGNED_CERT_FILENAME, DCF_Filetype_PEM, TSP_Profile_BCP_195_RFC_8996);
    OFCHECK(result.good());
    OFCHECK(scpTlsLayer.checkPrivateKeyMatchesCertificate());
    scpTlsLayer.setCertificateVerification(DCV_ignoreCertificate);

    /// Init and run Scp server with tls
    OFRandom rnd;
    TestPool pool;
    DcmSCPConfig& config = pool.getConfig();
    config.setAETitle("ACCEPTOR");
    config.setACSETimeout(30);
    config.setConnectionTimeout(1);
    config.setMaxReceivePDULength(16856);
    config.setHostLookupEnabled(false);
    config.setConnectionBlockingMode(DUL_NOBLOCK);
    config.setRespondWithCalledAETitle("REQUESTOR");
    OFList<OFString> xfers;
    xfers.push_back(UID_LittleEndianImplicitTransferSyntax);
    OFCHECK(config.addPresentationContext(UID_VerificationSOPClass, xfers, ASC_SC_ROLE_DEFAULT).good());
    config.setTransportLayer(&scpTlsLayer);
    pool.setMaxThreads(20);

    // Ensure server is up and listening
    int i = 0;
    Uint16 port_number = 0;
    OFMutex memory_barrier;
    do
    {
      // generate a random port number between 61440 (0xF000) and 65535
      port_number = 0xF000 + (rnd.getRND16() & 0xFFF);
      config.setPort(port_number);
      pool.start();
      force_sleep(2); // wait 2 seconds for the SCP process to start
      memory_barrier.lock();
      memory_barrier.unlock();
    }
    while ((i++ < 5) && (! pool.m_is_running)); // try up to 5 port numbers before giving up
    if (! pool.m_is_running) BAILOUT("Start of the SCP thread ppol failed: " << pool.m_listen_result.text());

    OFVector<TestTLSSCU*> scus(20);
    OFVector<DcmTLSTransportLayer*> scuTlsLayers;
    for (OFVector<TestTLSSCU*>::iterator it1 = scus.begin(); it1 != scus.end(); ++it1)
    {
        *it1 = new TestTLSSCU;
        (*it1)->setMaxReceivePDULength(16856);
        (*it1)->setDIMSEBlockingMode(DIMSE_NONBLOCKING);
        (*it1)->setDIMSETimeout(300);
        (*it1)->setACSETimeout(30);
        (*it1)->setPeerAETitle("ACCEPTOR");
        (*it1)->setAETitle("REQUESTOR");
        (*it1)->setPeerHostName("localhost");
        (*it1)->setPeerPort(port_number);
        (*it1)->enableAuthentication(PRIVATE_KEY_FILENAME, PUBLIC_SELFSIGNED_CERT_FILENAME, PRIVATE_KEY_PWD, DCF_Filetype_PEM, DCF_Filetype_PEM);
        (*it1)->setPeerCertVerification(DCV_ignoreCertificate);

        OFList<OFString> ts;
        ts.push_back(UID_LittleEndianImplicitTransferSyntax);
        OFCHECK((*it1)->addPresentationContext(UID_VerificationSOPClass, ts, ASC_SC_ROLE_SCU).good());
        (*it1)->initNetwork();
    }

    // "ensure" the pool is initialized before any SCU starts connecting to it. The initialization
    // can take a couple of seconds on older systems, e.g. debian i368.
    force_sleep(5);

    for (OFVector<TestTLSSCU*>::const_iterator it2 = scus.begin(); it2 != scus.end(); ++it2)
    {
        (*it2)->start();
    }

    for (OFVector<TestTLSSCU*>::iterator it3 = scus.begin(); it3 != scus.end(); ++it3)
    {
        (*it3)->join();
        OFCHECK((*it3)->result.good());
        delete *it3;
    }

    for (OFVector<DcmTLSTransportLayer*>::iterator it4 = scuTlsLayers.begin(); it4 != scuTlsLayers.end(); ++it4)
    {
        delete *it4;
    }

    pool.stopAfterCurrentAssociations();
    pool.join();
}

#endif // WITH_OPENSSL

#endif // WITH_THREADS

#if (!defined(WITH_THREADS)) || (!defined(WITH_OPENSSL))

// Dummy versions of the test cases. Needed to prevent ctest test failures.
OFTEST(dcmtls_scp_tls)
{
}

OFTEST(dcmtls_scp_pool_tls)
{
}

// This dummy function creates a dependency on libdcmnet that is required when compiling
// on NetBSD with libwrap support enabled and OpenSSL support disabled. Otherwise there
// would be a linker error complaining about unresolved symbols allow_severity and deny_severity.

DcmTransportLayer *tscuscptls_dummyFunction()
{
  return new DcmTransportLayer();
}

#endif
