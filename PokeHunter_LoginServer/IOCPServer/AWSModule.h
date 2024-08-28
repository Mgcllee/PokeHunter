#pragma once

#include <aws/core/Aws.h>
#include <aws/core/utils/Outcome.h>
#include <aws/core/utils/StringUtils.h>
#include <aws/core/utils/json/JsonSerializer.h>
#include <aws/core/client/ClientConfiguration.h>
#include <aws/core/auth/AWSCredentialsProviderChain.h>
#include <aws/cognito-idp/model/GetUserResult.h>
#include <aws/cognito-idp/model/GetUserRequest.h>
#include <aws/cognito-idp/CognitoIdentityProviderClient.h>

using namespace Aws::Utils;
using namespace Aws::CognitoIdentityProvider;
using namespace Aws::CognitoIdentityProvider::Model;

std::string GetPlayerName(std::string& AWSIdToken);
