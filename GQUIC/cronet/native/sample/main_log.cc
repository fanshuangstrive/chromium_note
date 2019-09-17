// Copyright 2018 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <iostream>

#include "cronet_c.h"
#include "sample_executor.h"
#include "sample_url_request_callback.h"
#include "base/strings/stringprintf.h"

Cronet_EnginePtr CreateCronetEngine() {
  Cronet_EnginePtr cronet_engine = Cronet_Engine_Create();
  Cronet_EngineParamsPtr engine_params = Cronet_EngineParams_Create();
  Cronet_EngineParams_user_agent_set(engine_params, "CronetSample/1");

  //Add quic_version.
  std::string quic_version =  "QUIC_VERSION_43";
  Cronet_EngineParams_experimental_options_set(
           engine_params,
             base::StringPrintf(
                  "{ \"QUIC\": { \"quic_version\" : \"%s\" ,\
		  \"connection_options\":\"TIME,TBBR,REJ\",\
		  \"client_connection_options\":\"TBBR,1RTT\" \
		  } }",
                          quic_version.c_str())
                         .c_str()) ;


  Cronet_EngineParams_enable_quic_set(engine_params, true);

  // Add QUIC Hint.
  Cronet_QuicHintPtr quic_hint = Cronet_QuicHint_Create();
  Cronet_QuicHint_host_set(quic_hint, "www.example.org");
  Cronet_QuicHint_port_set(quic_hint, 25001);
  Cronet_QuicHint_alternate_port_set(quic_hint, 25001);
  Cronet_EngineParams_quic_hints_add(engine_params, quic_hint);
  Cronet_QuicHint_Destroy(quic_hint);


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
  Cronet_UrlRequestParams_http_method_set(request_params, "GET");

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
  std::cout << "Hello from Cronet!\n";
  Cronet_EnginePtr cronet_engine = CreateCronetEngine();
  std::cout << "Cronet version: "
            << Cronet_Engine_GetVersionString(cronet_engine) << std::endl;

  Cronet_Engine_StartNetLogToFile(cronet_engine,"./1.log",true);//added by fs  log

  std::string url(argc > 1 ? argv[1] : "https://www.example.com");
  std::cout << "URL: " << url << std::endl;
  SampleExecutor executor;
  PerformRequest(cronet_engine, url, executor.GetExecutor());

  Cronet_Engine_StopNetLog(cronet_engine);////added by fs  log
  Cronet_Engine_Shutdown(cronet_engine);
  Cronet_Engine_Destroy(cronet_engine);

  return 0;
}
