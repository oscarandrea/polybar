#pragma once

#include <cstring>
#include <iomanip>
#include <iostream>
#include <map>

#include "common.hpp"

LEMONBUDDY_NS

namespace command_line {

  class option;
  using choices = vector<string>;
  using options = vector<option>;
  using values = map<string, string>;

  // class definition : option {{{

  class option {
   public:
    string flag;
    string flag_long;
    string desc;
    string token;
    choices values;

    /**
     * Construct option
     */
    explicit option(
        string&& flag, string&& flag_long, string&& desc, string&& token = "", choices&& c = {})
        : flag(forward<string>(flag))
        , flag_long(forward<string>(flag_long))
        , desc(forward<string>(desc))
        , token(forward<string>(token))
        , values(forward<choices>(c)) {}
  };

  // }}}
  // class definition : parser {{{

  class parser {
   public:
    /**
     * Construct parser
     */
    explicit parser(string&& synopsis, options&& opts)
        : m_synopsis(forward<string>(synopsis)), m_opts(forward<options>(opts)) {}

    /**
     * Process input values
     *
     * This is done outside the constructor due to boost::di noexcept
     */
    void process_input(const vector<string>& values) {
      for (size_t i = 0; i < values.size(); i++) {
        parse(values[i], values.size() > i + 1 ? values[i + 1] : "");
      }
    }

    /**
     * Test if the passed option was provided
     */
    bool has(string&& option) const {
      return m_optvalues.find(forward<string>(option)) != m_optvalues.end();
    }

    /**
     * Compares the option value with given string
     */
    bool compare(string&& opt, string val) const {
      return get(forward<string>(opt)) == val;
    }

    /**
     * Gets the value defined for given option
     */
    string get(string&& opt) const {
      if (has(forward<string>(opt)))
        return m_optvalues.find(forward<string>(opt))->second;
      return "";
    }

    /**
     * Prints application usage message
     */
    void usage() const {
      std::cout << m_synopsis << "\n" << std::endl;

      // get the length of the longest string in the flag column
      // which is used to align the description fields
      size_t maxlen{0};

      for (auto it = m_opts.begin(); it != m_opts.end(); ++it) {
        size_t len{it->flag_long.length() + it->flag.length() + 4};
        maxlen = len > maxlen ? len : maxlen;
      }

      for (auto& opt : m_opts) {
        int pad = maxlen - opt.flag_long.length() - opt.token.length();

        std::cout << "  " << opt.flag << ", " << opt.flag_long;

        if (!opt.token.empty()) {
          std::cout << "=" << opt.token;
          pad--;
        }

        // output the list with accepted values
        if (!opt.values.empty()) {
          std::cout << std::setw(pad + opt.desc.length()) << std::setfill(' ') << opt.desc
                    << std::endl;

          pad = pad + opt.flag_long.length() + opt.token.length() + 7;

          std::cout << string(pad, ' ') << opt.token << " is one of: ";

          for (auto& v : opt.values) {
            std::cout << v << (v != opt.values.back() ? ", " : "");
          }
        } else {
          std::cout << std::setw(pad + opt.desc.length()) << std::setfill(' ') << opt.desc;
        }

        std::cout << std::endl;
      }
    }

    /**
     * Configure injection module
     */
    template <class T = parser>
    static di::injector<T> configure(string scriptname, options opts) {
      // clang-format off
          return di::make_injector(
              di::bind<>().to("Usage: " + scriptname + " bar_name [OPTION...]"),
              di::bind<>().to(opts));
      // clang-format on
    }

   protected:
    /**
     * Compare option with its short version
     */
    auto is_short(string option, string opt_short) const {
      return option.compare(0, opt_short.length(), opt_short) == 0;
    }

    /**
     * Compare option with its long version
     */
    auto is_long(string option, string opt_long) const {
      return option.compare(0, opt_long.length(), opt_long) == 0;
    }

    /**
     * Compare option with both versions
     */
    auto is(string option, string opt_short, string opt_long) const {
      return is_short(option, opt_short) || is_long(option, opt_long);
    }

    /**
     * Gets value defined for
     */
    auto parse_value(string input, string input_next, choices values) const {
      string opt = input;
      size_t pos;
      string value;

      if (input_next.empty() && opt.compare(0, 2, "--") != 0)
        throw application_error("Missing value for " + opt);
      else if (!input_next.empty())
        value = input_next;
      else if ((pos = opt.compare(0, 1, "=")) == string::npos)
        throw application_error("Missing value for " + opt);
      else {
        value = opt.substr(pos + 1);
        opt = opt.substr(0, pos);
      }

      if (!values.empty() && std::find(values.begin(), values.end(), value) == values.end())
        throw application_error("Invalid argument '" + value + "' for " + string{opt});

      return value;
    }

    /**
     * Parses and validates passed arguments and flags
     */
    void parse(string input, string input_next = "") {
      if (m_skipnext) {
        m_skipnext = false;
        if (!input_next.empty())
          return;
      }

      for (auto&& opt : m_opts) {
        if (is(input, opt.flag, opt.flag_long)) {
          if (opt.token.empty()) {
            m_optvalues.insert(std::make_pair(opt.flag_long.substr(2), ""));
          } else {
            auto value = parse_value(input, input_next, opt.values);
            m_skipnext = (value == input_next);
            m_optvalues.insert(std::make_pair(opt.flag_long.substr(2), value));
          }

          return;
        }
      }

      if (input.compare(0, 1, "-") == 0) {
        throw application_error("Unrecognized option " + input);
      }
    }

   private:
    string m_synopsis;
    options m_opts;
    values m_optvalues;
    bool m_skipnext = false;
  };

  // }}}
}

LEMONBUDDY_NS_END
