// Copyright (C) 2022 Check Point Software Technologies Ltd. All rights reserved.

// Licensed under the Apache License, Version 2.0 (the "License");
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "new_log_trigger.h"

using namespace std;

USE_DEBUG_FLAG(D_LOCAL_POLICY);
// LCOV_EXCL_START Reason: no test exist

static const set<string> valid_severities = {"high", "critical"};
static const set<string> valid_protocols = {"tcp", "udp"};
static const set<string> valid_formats = {"json", "json-formatted"};

void
NewAppsecTriggerAccessControlLogging::load(cereal::JSONInputArchive &archive_in)
{
    dbgTrace(D_LOCAL_POLICY) << "Loading AppSec Trigger - Access Control Logging";
    parseAppsecJSONKey<bool>("allowEvents", ac_allow_events, archive_in, false);
    parseAppsecJSONKey<bool>("dropEvents", ac_drop_events, archive_in, true);
}

void
NewAppsecTriggerAdditionalSuspiciousEventsLogging::load(cereal::JSONInputArchive &archive_in)
{
    dbgTrace(D_LOCAL_POLICY) << "Loading AppSec Trigger - Additional Suspicious Events Logging";
    parseAppsecJSONKey<bool>("enabled", enabled, archive_in, true);
    parseAppsecJSONKey<bool>("responseBody", response_body, archive_in, false);
    parseAppsecJSONKey<bool>("responseCode", response_code, archive_in, true);
    parseAppsecJSONKey<string>("minSeverity", minimum_severity, archive_in, "high");
    if (valid_severities.count(minimum_severity) == 0) {
        dbgWarning(D_LOCAL_POLICY)
            << "AppSec AppSec Trigger - Additional Suspicious Events Logging minimum severity invalid: "
            << minimum_severity;
        throw PolicyGenException(
            "AppSec AppSec Trigger - Additional Suspicious Events Logging minimum severity invalid: "
            + minimum_severity
        );
    }
}

bool
NewAppsecTriggerAdditionalSuspiciousEventsLogging::isEnabled() const
{
    return enabled;
}

bool
NewAppsecTriggerAdditionalSuspiciousEventsLogging::isResponseBody() const
{
    return response_body;
}

const string &
NewAppsecTriggerAdditionalSuspiciousEventsLogging::getMinimumSeverity() const
{
    return minimum_severity;
}

void
NewAppsecTriggerLogging::load(cereal::JSONInputArchive &archive_in)
{
    dbgTrace(D_LOCAL_POLICY) << "Loading AppSec Trigger Logging";
    parseAppsecJSONKey<bool>("detectEvents", detect_events, archive_in, false);
    parseAppsecJSONKey<bool>("preventEvents", prevent_events, archive_in, true);
    parseAppsecJSONKey<bool>("allWebRequests", all_web_requests, archive_in, false);
}

bool
NewAppsecTriggerLogging::isAllWebRequests() const
{
    return all_web_requests;
}

bool
NewAppsecTriggerLogging::isDetectEvents() const
{
    return detect_events;
}

bool
NewAppsecTriggerLogging::isPreventEvents() const
{
    return prevent_events;
}

void
NewAppsecTriggerExtendedLogging::load(cereal::JSONInputArchive &archive_in)
{
    dbgTrace(D_LOCAL_POLICY) << "Loading AppSec Trigger Extended Logging";
    parseAppsecJSONKey<bool>("httpHeaders", http_headers, archive_in, false);
    parseAppsecJSONKey<bool>("requestBody", request_body, archive_in, false);
    parseAppsecJSONKey<bool>("urlPath", url_path, archive_in, false);
    parseAppsecJSONKey<bool>("urlQuery", url_query, archive_in, false);
}

bool
NewAppsecTriggerExtendedLogging::isHttpHeaders() const
{
    return http_headers;
}

bool
NewAppsecTriggerExtendedLogging::isRequestBody() const
{
    return request_body;
}

bool
NewAppsecTriggerExtendedLogging::isUrlPath() const
{
    return url_path;
}

bool
NewAppsecTriggerExtendedLogging::isUrlQuery() const
{
    return url_query;
}

void
NewLoggingService::load(cereal::JSONInputArchive &archive_in)
{
    parseAppsecJSONKey<string>("address", address, archive_in);
    parseAppsecJSONKey<string>("proto", proto, archive_in, "tcp");
    if (valid_protocols.count(proto) == 0) {
        dbgWarning(D_LOCAL_POLICY) << "AppSec Logging Service - proto invalid: " << proto;
        throw PolicyGenException("AppSec Logging Service - proto invalid: " + proto);
    }

    parseAppsecJSONKey<int>("port", port, archive_in, 514);
}

const string &
NewLoggingService::getAddress() const
{
    return address;
}

int
NewLoggingService::getPort() const
{
    return port;
}


void
NewStdoutLogging::load(cereal::JSONInputArchive &archive_in)
{
    parseAppsecJSONKey<string>("format", format, archive_in, "json");
    if (valid_formats.count(format) == 0) {
        dbgWarning(D_LOCAL_POLICY) << "AppSec Stdout Logging - format invalid: " << format;
        throw PolicyGenException("AppSec Stdout Logging - format invalid: " + format);
    }
}

const string &
NewStdoutLogging::getFormat() const
{
    return format;
}

void
NewAppsecTriggerLogDestination::load(cereal::JSONInputArchive &archive_in)
{
    dbgTrace(D_LOCAL_POLICY) << "Loading AppSec Trigger LogDestination";
    if (getConfigurationFlag("orchestration-mode") != "hybrid_mode") {
        // TBD: support "file"
        parseAppsecJSONKey<bool>("cloud", cloud, archive_in, false);
    } else {
        cloud = false;
    }
    bool local_tuning_default = false;
    // check ENV VAR LOCAL_TUNING_ENABLED
    char * tuning_enabled = getenv("LOCAL_TUNING_ENABLED");
    if (tuning_enabled != NULL) {
        for (unsigned int i = 0; i < strlen(tuning_enabled); i++) {
            tuning_enabled[i] = tolower(tuning_enabled[i]);
        }
        local_tuning_default = string(tuning_enabled) == "true";
    }
    parseAppsecJSONKey<bool>("local-tuning", container_service, archive_in, local_tuning_default);

    NewStdoutLogging stdout_log;
    parseAppsecJSONKey<NewStdoutLogging>("stdout", stdout_log, archive_in);
    parseAppsecJSONKey<bool>("logToAgent", agent_local, archive_in, true);
    beautify_logs = stdout_log.getFormat() == "json-formatted";
    parseAppsecJSONKey<NewLoggingService>("syslogService", syslog_service, archive_in);
    parseAppsecJSONKey<NewLoggingService>("cefService", cef_service, archive_in);
}

int
NewAppsecTriggerLogDestination::getCefServerUdpPort() const
{
    return getCefServiceData().getPort();
}

int
NewAppsecTriggerLogDestination::getSyslogServerUdpPort() const
{
    return getSyslogServiceData().getPort();
}

bool
NewAppsecTriggerLogDestination::isAgentLocal() const
{
    return agent_local;
}

bool
NewAppsecTriggerLogDestination::shouldBeautifyLogs() const
{
    return beautify_logs;
}

bool
NewAppsecTriggerLogDestination::getCloud() const
{
    return cloud;
}

bool
NewAppsecTriggerLogDestination::isContainerNeeded() const
{
    return container_service;
}

bool
NewAppsecTriggerLogDestination::isCefNeeded() const
{
    return !getCefServiceData().getAddress().empty();
}

bool
NewAppsecTriggerLogDestination::isSyslogNeeded() const
{
    return !getSyslogServiceData().getAddress().empty();
}

const
string & NewAppsecTriggerLogDestination::getSyslogServerIpv4Address() const
{
    return getSyslogServiceData().getAddress();
}

const string &
NewAppsecTriggerLogDestination::getCefServerIpv4Address() const
{
    return getCefServiceData().getAddress();
}

const NewLoggingService &
NewAppsecTriggerLogDestination::getSyslogServiceData() const
{
    return syslog_service;
}

const NewLoggingService &
NewAppsecTriggerLogDestination::getCefServiceData() const
{
    return cef_service;
}

void
NewAppsecLogTrigger::load(cereal::JSONInputArchive &archive_in)
{
    dbgTrace(D_LOCAL_POLICY) << "Loading AppSec log trigger";
    parseAppsecJSONKey<string>("appsecClassName", appsec_class_name, archive_in);
    parseMandatoryAppsecJSONKey<NewAppsecTriggerAccessControlLogging>(
        "accessControlLogging",
        access_control_logging,
        archive_in
    );
    parseMandatoryAppsecJSONKey<NewAppsecTriggerAdditionalSuspiciousEventsLogging>(
        "additionalSuspiciousEventsLogging",
        additional_suspicious_events_logging,
        archive_in
    );
    parseMandatoryAppsecJSONKey<NewAppsecTriggerLogging>("appsecLogging", appsec_logging, archive_in);
    parseMandatoryAppsecJSONKey<NewAppsecTriggerExtendedLogging>("extendedLogging", extended_logging, archive_in);
    parseMandatoryAppsecJSONKey<NewAppsecTriggerLogDestination>("logDestination", log_destination, archive_in);
    parseAppsecJSONKey<string>("name", name, archive_in);
}

void
NewAppsecLogTrigger::setName(const string &_name)
{
    name = _name;
}

const string &
NewAppsecLogTrigger::getName() const
{
    return name;
}

const string &
NewAppsecLogTrigger::getAppSecClassName() const
{
    return appsec_class_name;
}

const NewAppsecTriggerAdditionalSuspiciousEventsLogging &
NewAppsecLogTrigger::getAppsecTriggerAdditionalSuspiciousEventsLogging() const
{
    return additional_suspicious_events_logging;
}

const NewAppsecTriggerLogging &
NewAppsecLogTrigger::getAppsecTriggerLogging() const
{
    return appsec_logging;
}

const NewAppsecTriggerAccessControlLogging &
NewAppsecLogTrigger::getAppsecTriggerAccessControlLogging() const
{
    return access_control_logging;
}


const NewAppsecTriggerExtendedLogging &
NewAppsecLogTrigger::getAppsecTriggerExtendedLogging() const
{
    return extended_logging;
}

const NewAppsecTriggerLogDestination &
NewAppsecLogTrigger::getAppsecTriggerLogDestination() const
{
    return log_destination;
}
// LCOV_EXCL_STOP
