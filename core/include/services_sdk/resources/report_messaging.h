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

#ifndef __REPORT_MESSAGING_H__
#define __REPORT_MESSAGING_H__

#include "singleton.h"
#include "i_time_get.h"
#include "i_messaging.h"
#include "report/report.h"

class ReportMessaging
        :
    Singleton::Consume<I_Messaging>,
    Singleton::Consume<I_TimeGet>
{
public:
    template <typename ...Args, typename T>
    ReportMessaging(
        const std::string &title,
        const ReportIS::AudienceTeam &audience_team,
        const T &obj,
        Args ...args)
            :
        ReportMessaging(
            title,
            audience_team,
            obj,
            MessageCategory::GENERIC,
            std::forward<Args>(args)...
        )
    {
    }

    template <typename ...Args, typename T>
    ReportMessaging(
        const std::string &title,
        const ReportIS::AudienceTeam &audience_team,
        const T &obj,
        const MessageCategory &message_type,
        Args ...args)
            :
        ReportMessaging(
            title,
            audience_team,
            ReportIS::Severity::INFO,
            ReportIS::Priority::LOW,
            obj,
            message_type,
            std::forward<Args>(args)...
        )
    {
    }

    template <typename ...Args, typename T>
    ReportMessaging(
        const std::string &title,
        const ReportIS::AudienceTeam &audience_team,
        const ReportIS::Severity &severity,
        const ReportIS::Priority &priority,
        const T &obj,
        Args ...args)
            :
        ReportMessaging(
            title,
            audience_team,
            severity,
            priority,
            obj,
            MessageCategory::GENERIC,
            std::forward<Args>(args)...
        )
    {
    }


    template <typename ...Args, typename T>
    ReportMessaging(
        const std::string &title,
        const ReportIS::AudienceTeam &audience_team,
        const ReportIS::Severity &severity,
        const ReportIS::Priority &priority,
        const T &obj,
        const MessageCategory &message_type,
        Args ...args)
            :
        report(
            title,
            Singleton::Consume<I_TimeGet>::by<ReportMessaging>()->getWalltime(),
            ReportIS::Type::EVENT,
            ReportIS::Level::LOG,
            ReportIS::LogLevel::INFO,
            ReportIS::Audience::INTERNAL,
            audience_team,
            severity,
            priority,
            std::chrono::seconds(0),
            std::forward<Args>(args)...
        ),
        message_type_tag(message_type)
    {
        report << LogField("eventObject", obj);
    }

    ~ReportMessaging();

    ReportMessaging & operator<<(const LogField &field);

    Maybe<void, HTTPResponse> sendReportSynchronously();

    void setForceBuffering(bool _force_buffering);

private:
    Report report;
    bool is_async_message = true;
    bool force_buffering = false;
    MessageCategory message_type_tag;
};

#endif // __REPORT_MESSAGING_H__
