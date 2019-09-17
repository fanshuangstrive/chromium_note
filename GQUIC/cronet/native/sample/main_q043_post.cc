// Copyright 2018 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <iostream>
#include <fstream>  
#include <sstream>
#include "cronet_c.h"
#include "sample_executor.h"
#include "sample_url_request_callback.h"

#include "base/strings/stringprintf.h"
#include "net/cert/mock_cert_verifier.h"
#include "components/cronet/native/test/test_upload_data_provider.h"

std::string StrPostData; 
std::string FileName; 

Cronet_EnginePtr CreateCronetEngine() {
  Cronet_EnginePtr cronet_engine = Cronet_Engine_Create();
  Cronet_EngineParamsPtr engine_params = Cronet_EngineParams_Create();
  Cronet_EngineParams_user_agent_set(engine_params, "CronetSample/1");
  
  Cronet_EngineParams_enable_http2_set(engine_params,true);//add fs 
  Cronet_EngineParams_enable_quic_set(engine_params, true);

  Cronet_Engine_StartWithParams(cronet_engine, engine_params);
  Cronet_EngineParams_Destroy(engine_params);
  return cronet_engine;
}
// Test Cert Verifier that successfully verifies any cert from test.example.com.
/*
class TestCertVerifier : public net::MockCertVerifier {
 public:
  TestCertVerifier() = default;
  ~TestCertVerifier() override = default;

  // CertVerifier implementation
  int Verify(const RequestParams& params,
             net::CertVerifyResult* verify_result,
             net::CompletionOnceCallback callback,
             std::unique_ptr<Request>* out_req,
             const net::NetLogWithSource& net_log) override {
    if (params.hostname() == "www.example.org") {
      verify_result->verified_cert = params.certificate();
      verify_result->cert_status = MapNetErrorToCertStatus(net::OK);
      return net::OK;
    }
    return net::MockCertVerifier::Verify(params, verify_result,
                                         std::move(callback), out_req, net_log);
  }
};
*/

Cronet_EnginePtr CreateCronetEngine(int quic_server_port) {
  Cronet_EngineParamsPtr engine_params = Cronet_EngineParams_Create();
  Cronet_EngineParams_user_agent_set(engine_params, "testfs");
  // // Add Host Resolver Rules.
  // std::string host_resolver_rules = base::StringPrintf(
  //     "MAP example.com 192.168.1.103:%d,"
  //     "MAP example.com ~NOTFOUND",
  //     quic_server_port);
  // Cronet_EngineParams_experimental_options_set(
  //     engine_params,
  //     base::StringPrintf(
  //         "{ \"HostResolverRules\": { \"host_resolver_rules\" : \"%s\" } }",
  //         host_resolver_rules.c_str())
  //         .c_str());


   // Add quic_version.
  std::string quic_version =  "QUIC_VERSION_43";
  Cronet_EngineParams_experimental_options_set(
      engine_params,
          base::StringPrintf(
          "{ \"QUIC\": { \"quic_version\" : \"%s\"\
 	    }}",
          quic_version.c_str())
          .c_str()) ;
  

  //
  // \"connection_options\":\"TIME,TBBR,REJ\,
  // \"client_connection_options\":\"TBBR,1RTT\" 
  std::string kQUIC="QUIC"; 
  /********Cronet_EngineParams_experimental_options_set(
      engine_params,
          base::StringPrintf(
          "{ \"QUIC\": { \"quic_version\" : \"%s\",\
   	    \"idle_connection_timeout_seconds\": 30,\
   	    \"host_whitelist\": \"www.example.org\",\
   	    \"race_cert_verification\": true,\
   	    \"support_ietf_format_quic_altsvc\": true,\
   	    \"enable_socket_recv_optimization\": true,\
   	    \"connection_options\":\"TIME,TBBR,REJ\",\
   	    \"client_connection_options\":\"TBBR,1RTT\" \
   	    }}",
          quic_version.c_str())
          .c_str()) ;
  *********/
	
  std::cout << "options: "<< Cronet_EngineParams_experimental_options_get(engine_params) 
            << std::endl;

  Cronet_EngineParams_enable_http2_set(engine_params,true);//add fs 
  // Enable QUIC.
  Cronet_EngineParams_enable_quic_set(engine_params, true);

  Cronet_EngineParams_enable_brotli_set(engine_params, true);
  //Cronet_EngineParams_http_cache_mode_set(engine_params,Cronet_EngineParams_HTTP_CACHE_MODE_IN_MEMORY);
  //Cronet_EngineParams_http_cache_max_size_set(engine_params,10*1024*1024);

  // Add QUIC Hint.
  Cronet_QuicHintPtr quic_hint = Cronet_QuicHint_Create();
  Cronet_QuicHint_host_set(quic_hint, "example.com");
  Cronet_QuicHint_port_set(quic_hint, 443);
  Cronet_QuicHint_alternate_port_set(quic_hint, 443);
  Cronet_EngineParams_quic_hints_add(engine_params, quic_hint);
  Cronet_QuicHint_Destroy(quic_hint);



  // Create Cronet Engine.
  Cronet_EnginePtr cronet_engine = Cronet_Engine_Create();

  // Set Mock Cert Verifier.
  //auto cert_verifier = std::make_unique<TestCertVerifier>();
  //Cronet_Engine_SetMockCertVerifierForTesting(cronet_engine,
  //                                           cert_verifier.release());
  //https://cs.chromium.org/chromium/src/components/cronet/native/perftest/perf_test.cc?q=SetMockCertVerifierForTesting&dr=C 
  // auto cert_verifier = std::make_unique<net::MockCertVerifier>();
  // cert_verifier->set_default_result(net::OK);
  // Cronet_Engine_SetMockCertVerifierForTesting(cronet_engine,
  //                                                cert_verifier.release());
                                             
  // Start Cronet Engine.
  Cronet_Engine_StartWithParams(cronet_engine, engine_params);
  Cronet_EngineParams_Destroy(engine_params);
  return cronet_engine;
}

void PerformRequest(Cronet_EnginePtr cronet_engine,
                    const std::string& url,
                    Cronet_ExecutorPtr executor) {
                    

  SampleUrlRequestCallback url_request_callback;
  Cronet_UrlRequestPtr request = Cronet_UrlRequest_Create();
  Cronet_UrlRequestParamsPtr request_params = Cronet_UrlRequestParams_Create();
  // Cronet_UrlRequestParams_http_method_set(request_params, "GET");
  Cronet_UrlRequestParams_http_method_set(request_params, "POST");
  //Cronet_UrlRequestParams_priority_set(request_params, Cronet_UrlRequestParams_REQUEST_PRIORITY_REQUEST_PRIORITY_LOW);
  //Cronet_UrlRequestParams_priority_set(request_params, Cronet_UrlRequestParams_REQUEST_PRIORITY_REQUEST_PRIORITY_HIGHEST);
  Cronet_HttpHeaderPtr header = Cronet_HttpHeader_Create();
  Cronet_HttpHeader_name_set(header, "Content-Type");
  Cronet_HttpHeader_value_set(header, "multipart/form-data; boundary=----WebKitFormBoundaryP32H0UxDXLvTCJmA");
  Cronet_UrlRequestParams_request_headers_add(request_params, header);
  Cronet_HttpHeader_Destroy(header);

  Cronet_UploadDataProviderPtr upload_data_provider = nullptr;
  // Add upload data provider and set content type required for upload.
  cronet::test::TestUploadDataProvider data_provider(cronet::test::TestUploadDataProvider::SYNC,
		                                         executor);                                         
  upload_data_provider = data_provider.CreateUploadDataProvider();
  data_provider.AddRead("------WebKitFormBoundaryP32H0UxDXLvTCJmA\r\n");
  if(FileName.length()>0){
    data_provider.AddRead("Content-Disposition: form-data; name=\"file\"; filename=");
    data_provider.AddRead(FileName.c_str() );
    data_provider.AddRead(" \r\n");
  }else{
    data_provider.AddRead("Content-Disposition: form-data; name=\"file\"; filename=1.txt \r\n");
  }
  data_provider.AddRead("Content-Type: text/plain\r\n");
  data_provider.AddRead("\r\n");
  if(StrPostData.length()>0){
    data_provider.AddRead(StrPostData.c_str());
  }else{
    //data_provider.AddRead("");
  }
  data_provider.AddRead("\r\n");
  data_provider.AddRead("------WebKitFormBoundaryP32H0UxDXLvTCJmA--\r\n");
  Cronet_UrlRequestParams_upload_data_provider_set(request_params,upload_data_provider);
  Cronet_UrlRequestParams_upload_data_provider_executor_set(
			            request_params, data_provider.executor());
  Cronet_UrlRequestParams_upload_data_provider_set(request_params,upload_data_provider);


  Cronet_UrlRequest_InitWithParams(
      request, cronet_engine, url.c_str(), request_params,
      url_request_callback.GetUrlRequestCallback(), executor);
  Cronet_UrlRequestParams_Destroy(request_params);

  Cronet_UrlRequest_Start(request);
  url_request_callback.WaitForDone();
  Cronet_UrlRequest_Destroy(request);

  std::cout << "Response Data:" << std::endl
            << url_request_callback.response_as_string() << std::endl;
}

// Download a resource from the Internet. Optional argument must specify
// a valid URL.
int main(int argc, const char* argv[]) {
  std::cout << "Usage: ./cronet_sample_q043_post  \n";
  std::cout << "Usage: ./cronet_sample_q043_post <url> \n";
  std::cout << "Usage: ./cronet_sample_q043_post <url> <filename>\n";
  std::cout << "\n" ;
  std::cout << "example: ./cronet_sample_q043_post \"https://example.com/upload.php\" <filename>\n";
  std::cout << "\n" ;
  std::cout << "\n" ;
  std::cout << "Hello from Cronet!\n";

  if(argc > 2){
     std::cout << "file:"<<argv[2] <<std::endl;
     std::fstream fs( argv[2] ) ;
     std::stringstream ss ;
     ss << fs.rdbuf() ; 
     StrPostData = ss.str();
     FileName=std::string(argv[2]);
     fs.close();
  }

  Cronet_EnginePtr cronet_engine = CreateCronetEngine(443);
  
 // Cronet_Engine_StartNetLogToFile(cronet_engine,"./1.log",true);//added by fs  log

  std::cout << "Cronet version: "
            << Cronet_Engine_GetVersionString(cronet_engine) << std::endl;

  std::string url(argc > 1 ? argv[1] : "https://www.example.com");
  std::cout << "URL: " << url << std::endl;
  SampleExecutor executor;
  PerformRequest(cronet_engine, url, executor.GetExecutor());

  Cronet_Engine_Shutdown(cronet_engine);
  Cronet_Engine_Destroy(cronet_engine);

  //Cronet_Engine_StopNetLog(cronet_engine);////added by fs  log
  return 0;
}
