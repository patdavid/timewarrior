////////////////////////////////////////////////////////////////////////////////
//
// Copyright 2015 - 2016, Paul Beckingham, Federico Hernandez.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included
// in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
// OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
// THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//
// http://www.opensource.org/licenses/mit-license.php
//
////////////////////////////////////////////////////////////////////////////////

#include <cmake.h>
#include <timew.h>
#include <shared.h>
#include <Datetime.h>
#include <Duration.h>
#include <sstream>
#include <iomanip>
#include <map>
#include <vector>

////////////////////////////////////////////////////////////////////////////////
// Consult rules to find any defined color for the given tah, and colorize it.
Color tagColor (const Rules& rules, const std::string& tag)
{
  Color c;
  std::string name = std::string ("tags.") + tag + ".color";
  if (rules.has (name))
    c = Color (rules.get (name));

  return c;
}

////////////////////////////////////////////////////////////////////////////////
// Summarize either an active or closed interval, for user feedback.
std::string intervalSummarize (
  Database& database,
  const Rules& rules,
  const Interval& interval)
{
  std::stringstream out;

  if (interval.range.is_started ())
  {
    // Walk backwards through the inclusions, and stop as soon as the tags
    // no longer match interval. This means the 'total' is the sum of all time
    // in the most recent set of intervals for the same tags. This is the
    // acceptable definition of "the current task".
    time_t total_recorded = 0;
    auto inclusions = getAllInclusions (database);
    std::vector <Interval>::reverse_iterator i;
    for (i = inclusions.rbegin (); i != inclusions.rend (); i++)
      if (interval.tags () == i->tags ())
        total_recorded += i->range.total ();
      else
        break;

    Duration total (total_recorded);

    // Combine and colorize tags.
    std::string tags;
    for (auto& tag : interval.tags ())
    {
      if (tags != "")
        tags += " ";

      tags += tagColor (rules, tag).colorize (quoteIfNeeded (tag));
    }

    // Interval open.
    if (interval.range.is_open ())
    {
      out << "Tracking " << tags << '\n'
          << "  Started " << interval.range.start.toISOLocalExtended () << '\n'
          << "  Current " << Datetime ().toISOLocalExtended () << '\n'
          << "  Total   " << std::setw (19) << std::setfill (' ') << total.formatHours () << '\n';
    }

    // Interval closed.
    else
    {
      out << "Recorded " << tags << '\n'
          << "  Started " << interval.range.start.toISOLocalExtended () << '\n'
          << "  Ended   " << interval.range.end.toISOLocalExtended () << '\n'
          << "  Total   " << std::setw (19) << std::setfill (' ') << total.formatHours () << '\n';
    }
  }

  return out.str ();
}

////////////////////////////////////////////////////////////////////////////////
// Convert a set of hints to equivalent date ranges.
bool expandIntervalHint (
  const std::string& hint,
  std::string& start,
  std::string& end)
{
  static std::map <std::string, std::vector <std::string>> hints
  {
    {":yesterday",   {"yesterday", "today"}},
    {":day",         {"today",     "eod"}},
    {":week",        {"socw",      "eocw"}},
    {":month",       {"socm",      "eocm"}},
    {":quarter",     {"socq",      "eocq"}},
    {":year",        {"socy",      "eocy"}},
  };

  // Some hints are just synonyms.
  if (hints.find (hint) != hints.end ())
  {
    start = hints[hint][0];
    end   = hints[hint][1];
    return true;
  }

  // Some require math.
  if (hint == ":lastweek")
  {
    Datetime socw ("socw");
    Datetime eocw ("eocw");
    socw -= 7 * 86400;
    eocw -= 7 * 86400;
    start = socw.toString ("Y-M-D");
    end = eocw.toString ("Y-M-D");
  }

  return false;
}

////////////////////////////////////////////////////////////////////////////////
// Compose a JSON document of intervals. In the trivial case:
//   [
//   ]
//
// In the non-trivial case:
//   [
//   {...},
//   {...}
//   ]
//
std::string jsonFromIntervals (const std::vector <Interval>& intervals)
{
  std::stringstream out;

  out << "[\n";
  int counter = 0;
  for (auto& interval : intervals)
  {
    if (counter)
      out << ",\n";

    out << interval.json ();
    ++counter;
  }

  if (counter)
    out << '\n';

  out << "]\n";
  return out.str ();
}

////////////////////////////////////////////////////////////////////////////////
Palette createPalette (const Rules& rules)
{
  Palette p;
  auto colors = rules.all ("theme.palette.color");
  if (colors.size ())
  {
    p.clear ();
    for (auto& c : colors)
      p.add (Color (rules.get (c)));
  }

  p.enabled = rules.getBoolean ("color");
  return p;
}

////////////////////////////////////////////////////////////////////////////////
// Extract the tags from a set of intervals, and using a rotating color palette,
// map unique tags to color.
//
// If there is a tags.<tag>.color setting, use it.
std::map <std::string, Color> createTagColorMap (
  const Rules& rules,
  Palette& palette,
  const std::vector <Interval>& intervals)
{
  std::map <std::string, Color> mapping;
  for (auto& interval : intervals)
  {
    for (auto& tag : interval.tags ())
    {
      std::string custom = "tags." + tag + ".color";
      if (rules.has (custom))
        mapping[tag] = Color (rules.get (custom));
      else if (mapping.find (tag) == mapping.end ())
        mapping[tag] = palette.next ();
    }
  }

  return mapping;
}

////////////////////////////////////////////////////////////////////////////////
int quantizeTo15Minutes (const int minutes)
{
  if (minutes == 0  ||
      minutes == 15 ||
      minutes == 45 ||
      minutes == 60)
    return minutes;

  auto deviation = minutes % 15;
  if (deviation < 8)
    return minutes - deviation;

  return minutes + 15 - deviation;
}

////////////////////////////////////////////////////////////////////////////////
