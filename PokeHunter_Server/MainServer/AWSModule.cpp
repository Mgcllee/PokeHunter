#pragma once

#include "AWSModule.h"

#include <regex>

std::string GetPlayerName(std::string& AWSIdToken) {
  AWSIdToken = std::regex_replace(AWSIdToken, std::regex("\n"), "");
  Aws::SDKOptions options;
  Aws::InitAPI(options);
  {
    Aws::Client::ClientConfiguration clientConfig;
    clientConfig.region = "ap-northeast-2";

    Aws::CognitoIdentityProvider::CognitoIdentityProviderClient cognitoClient(
        clientConfig);

    Aws::CognitoIdentityProvider::Model::GetUserRequest request;
    request.WithAccessToken(AWSIdToken);

    auto outcome = cognitoClient.GetUser(request);

    // TODO: send log thread

    if (outcome.IsSuccess()) {
      Aws::CognitoIdentityProvider::Model::GetUserResult result =
          outcome.GetResult();
      return result.GetUsername();
    } else {
      return "Token Error";
    }
  }
  Aws::ShutdownAPI(options);

  return "Empty";
}