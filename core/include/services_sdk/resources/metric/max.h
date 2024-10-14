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

#ifndef __MAX_H__
#define __MAX_H__

#ifndef __GENERIC_METRIC_H__
#error metric/max.h should not be included directly
#endif // __GENERIC_METRIC_H_

namespace MetricCalculations
{

template <typename T>
class Max : public MetricCalc
{
public:
    Max(GenericMetric *metric, const std::string &title) : Max(metric, title, 0) {}

    template<typename ... Args>
    Max(GenericMetric *metric, const std::string &title, T min_val, const Args & ... args)
            :
        MetricCalc(metric, title, args ...),
        max(min_val),
        reset_value(min_val)
    {
    }

    void
    report(const T &new_value)
    {
        if (new_value > max || first) max = new_value;
        first = false;
    }

    void
    reset() override
    {
        max = reset_value;
        first = true;
    }

    T
    getMax() const
    {
        return max;
    }

    float
    getValue() const override
    {
        return static_cast<float>(max);
    }

    void
    save(cereal::JSONOutputArchive &ar) const override
    {
        ar(cereal::make_nvp(getMetricName(), max));
    }

    LogField
    getLogField() const override
    {
        return LogField(getMetricName(), static_cast<uint64_t>(getMax()));
    }

private:
    T max;
    T reset_value;
    bool first = true;
};

} // namespace MetricCalculations

#endif // __MAX_H__
