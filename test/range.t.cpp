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
#include <Range.h>
#include <test.h>

////////////////////////////////////////////////////////////////////////////////
int main (int, char**)
{
  UnitTest t (64);

  // bool started () const;
  // bool ended () const;
  Range i1;
  t.is (i1.started (), false, "Range().started -> false");
  t.is (i1.ended (),   false, "Range().ended -> false");

  // void start (Datetime);
  i1.start = Datetime ();
  t.is (i1.started (), true,  "Range(start=now).started -> true");
  t.is (i1.ended (),   false, "Range(start=now).ended -> false");

  // void end (Datetime);
  i1.end = Datetime ();
  t.is (i1.started (), true, "Range(start=now,end=now).started -> true");
  t.is (i1.ended (),   true, "Range(start=now,end=now).ended -> true");

  // this                     [--------)
  // A          [--------)
  // B                   [--------)
  // C                          [----|
  // D                             [--------)
  // E                                      [--------)
  // F                      [-------------)
  // G                      [...
  // H                           [...
  // I                                   [...
  Range refClosed;
  refClosed.start = Datetime (2016, 6,  1);
  refClosed.end   = Datetime (2016, 6, 30);

  Range testA; testA.start = Datetime (2016, 4,  1); testA.end = Datetime (2016, 4, 30);
  Range testB; testB.start = Datetime (2016, 5, 15); testB.end = Datetime (2016, 6, 15);
  Range testC; testC.start = Datetime (2016, 6, 10); testC.end = Datetime (2016, 6, 20);
  Range testD; testD.start = Datetime (2016, 6, 15); testD.end = Datetime (2016, 7, 15);
  Range testE; testE.start = Datetime (2016, 8,  1); testE.end = Datetime (2016, 8, 31);
  Range testF; testF.start = Datetime (2016, 5, 15); testF.end = Datetime (2016, 7, 15);
  Range testG; testG.start = Datetime (2016, 5, 15);
  Range testH; testH.start = Datetime (2016, 6, 15);
  Range testI; testI.start = Datetime (2016, 7, 15);

  t.notok (refClosed.overlap (testA), "Range: ! refClosed.overlap(testA)");
  t.ok    (refClosed.overlap (testB), "Range:   refClosed.overlap(testB)");
  t.ok    (refClosed.overlap (testC), "Range:   refClosed.overlap(testC)");
  t.ok    (refClosed.overlap (testD), "Range:   refClosed.overlap(testD)");
  t.notok (refClosed.overlap (testE), "Range: ! refClosed.overlap(testE)");
  t.ok    (refClosed.overlap (testF), "Range:   refClosed.overlap(testF)");
  t.ok    (refClosed.overlap (testG), "Range:   refClosed.overlap(testG)");
  t.ok    (refClosed.overlap (testH), "Range:   refClosed.overlap(testH)");
  t.notok (refClosed.overlap (testI), "Range: ! refClosed.overlap(testI)");

  // this                     [...
  // A          [--------)
  // B                   [--------)
  // C                          [----)
  // D                             [--------)
  // E                                      [--------)
  // F                      [-------------)
  // G                      [...
  // H                           [...
  // I                                   [...
  Range refOpen;
  refOpen.start = Datetime (2016, 6, 1);

  t.notok (refOpen.overlap (testA), "Range: ! refOpen.overlap(testA)");
  t.ok    (refOpen.overlap (testB), "Range:   refOpen.overlap(testB)");
  t.ok    (refOpen.overlap (testC), "Range:   refOpen.overlap(testC)");
  t.ok    (refOpen.overlap (testD), "Range:   refOpen.overlap(testD)");
  t.ok    (refOpen.overlap (testE), "Range:   refOpen.overlap(testE)");
  t.ok    (refOpen.overlap (testF), "Range:   refOpen.overlap(testF)");
  t.ok    (refOpen.overlap (testG), "Range:   refOpen.overlap(testG)");
  t.ok    (refOpen.overlap (testH), "Range:   refOpen.overlap(testH)");
  t.ok    (refOpen.overlap (testI), "Range:   refOpen.overlap(testI)");

  // this                     [--------)
  // A          [--------)
  // B                   [--------)
  // C                          [----)
  // D                             [--------)
  // E                                      [--------)
  // F                      [-------------)
  // G                      [...
  // H                           [...
  // I                                   [...
  Range empty;
  t.ok (refClosed.intersect (testA) == empty,                              "Range: refClosed.intersect(testA) == empty");
  t.ok (refClosed.intersect (testB) == Range (refClosed.start, testB.end), "Range: refClosed.intersect(testB) == Range(refClosed.start,testB.end)");
  t.ok (refClosed.intersect (testC) == testC,                              "Range: refClosed.intersect(testC) == testC");
  t.ok (refClosed.intersect (testD) == Range (testD.start, refClosed.end), "Range: refClosed.intersect(testD) == Range(testD.start,refClosed.end)");
  t.ok (refClosed.intersect (testE) == empty,                              "Range: refClosed.intersect(testE) == empty");
  t.ok (refClosed.intersect (testF) == refClosed,                          "Range: refClosed.intersect(testF) == refClosed");
  t.ok (refClosed.intersect (testG) == refClosed,                          "Range: refClosed.intersect(testG) == refClosed");
  t.ok (refClosed.intersect (testH) == Range (testH.start, refClosed.end), "Range: refClosed.intersect(testH) == Range(testH.start,refClosed.end)");
  t.ok (refClosed.intersect (testI) == empty,                              "Range: refClosed.intersect(testI) == empty");

  // this                     [...
  // A          [--------)
  // B                   [--------)
  // C                          [----)
  // D                             [--------)
  // E                                      [--------)
  // F                      [-------------)
  // G                      [...
  // H                           [...
  // I                                   [...
  t.ok (refOpen.intersect (testA) == empty,                            "Range: refOpen.intersect(testA) == empty");
  t.ok (refOpen.intersect (testB) == Range (refOpen.start, testB.end), "Range: refOpen.intersect(testB) == Range(refOpen.start,testB.end)");
  t.ok (refOpen.intersect (testC) == testC,                            "Range: refOpen.intersect(testC) == testC");
  t.ok (refOpen.intersect (testD) == testD,                            "Range: refOpen.intersect(testD) == testD");
  t.ok (refOpen.intersect (testE) == testE,                            "Range: refOpen.intersect(testE) == testE");
  t.ok (refOpen.intersect (testF) == Range (refOpen.start, testF.end), "Range: refOpen.intersect(testF) == Range(refOpen.start,testF.end");
  t.ok (refOpen.intersect (testG) == refOpen,                          "Range: refOpen.intersect(testG) == refOpen");
  t.ok (refOpen.intersect (testH) == testH,                            "Range: refOpen.intersect(testH) == testH");
  t.ok (refOpen.intersect (testI) == testI,                            "Range: refOpen.intersect(testI) == testI");

  // this                     [--------)
  // A          [--------)
  // B                   [--------)
  // C                          [----)
  // D                             [--------)
  // E                                      [--------)
  // F                      [-------------)
  // G                      [...
  // H                           [...
  // I                                   [...
  std::vector <Range> closedSubtractA {refClosed};
  std::vector <Range> closedSubtractB {Range (testB.end, refClosed.end)};
  std::vector <Range> closedSubtractC {Range (refClosed.start, testC.start), Range (testC.end, refClosed.end)};
  std::vector <Range> closedSubtractD {Range (refClosed.start, testD.start)};
  std::vector <Range> closedSubtractE {refClosed};
  std::vector <Range> closedSubtractF {};
  std::vector <Range> closedSubtractG {};
  std::vector <Range> closedSubtractH {Range (refClosed.start, testH.start)};
  std::vector <Range> closedSubtractI {refClosed};

  t.ok (refClosed.subtract (testA) == closedSubtractA, "Range: refClosed.subtract(testA) == {refClosed}");
  t.ok (refClosed.subtract (testB) == closedSubtractB, "Range: refClosed.subtract(testB) == {Range(testB.end,refClosed.end)}");
  t.ok (refClosed.subtract (testC) == closedSubtractC, "Range: refClosed.subtract(testC) == {Range(refClosed.start,testC.start),"
                                                                                            "Range(testC.end,refClosed.end}");
  t.ok (refClosed.subtract (testD) == closedSubtractD, "Range: refClosed.subtract(testD) == {Range(refClosed.start,testD.start)}");
  t.ok (refClosed.subtract (testE) == closedSubtractE, "Range: refClosed.subtract(testE) == {refClosed}");
  t.ok (refClosed.subtract (testF) == closedSubtractF, "Range: refClosed.subtract(testF) == {}");
  t.ok (refClosed.subtract (testG) == closedSubtractG, "Range: refClosed.subtract(testG) == {}");
  t.ok (refClosed.subtract (testH) == closedSubtractH, "Range: refClosed.subtract(testH) == {refClosed.start,testH.start}");
  t.ok (refClosed.subtract (testI) == closedSubtractI, "Range: refClosed.subtract(testI) == {refClosed}");

  // this                     [...
  // A          [--------)
  // B                   [--------)
  // C                          [----)
  // D                             [--------)
  // E                                      [--------)
  // F                      [-------------)
  // G                      [...
  // H                           [...
  // I                                   [...
  std::vector <Range> openSubtractA {refOpen};
  std::vector <Range> openSubtractB {Range (testB.end, refOpen.end)};
  std::vector <Range> openSubtractC {Range (refOpen.start, testC.start), Range (testC.end, refOpen.end)};
  std::vector <Range> openSubtractD {Range (refOpen.start, testD.start), Range (testD.end, refOpen.end)};
  std::vector <Range> openSubtractE {Range (refOpen.start, testE.start), Range (testE.end, refOpen.end)};
  std::vector <Range> openSubtractF {Range (testF.end, refOpen.end)};
  std::vector <Range> openSubtractG {};
  std::vector <Range> openSubtractH {Range (refOpen.start, testH.start)};
  std::vector <Range> openSubtractI {Range (refOpen.start, testI.start)};

  t.ok (refOpen.subtract (testA) == openSubtractA, "Range: refOpen.subtract(testA) == {refOpen}");
  t.ok (refOpen.subtract (testB) == openSubtractB, "Range: refOpen.subtract(testB) == {Range(testB.end,refOpen.end}");
  t.ok (refOpen.subtract (testC) == openSubtractC, "Range: refOpen.subtract(testC) == {Range(refOpen.start,testC.start),"
                                                                                      "Range(testC.end,refOpen.end}");
  t.ok (refOpen.subtract (testD) == openSubtractD, "Range: refOpen.subtract(testD) == {Range(refOpen.start,testD.start),"
                                                                                      "Range(testD.end,refOpen.end}");
  t.ok (refOpen.subtract (testE) == openSubtractE, "Range: refOpen.subtract(testE) == {Range(refOpen.start,testE.start),"
                                                                                      "Range(testE.end,refOpen.end}");
  t.ok (refOpen.subtract (testF) == openSubtractF, "Range: refOpen.subtract(testF) == {Range(testF.end,refOpen.end}");
  t.ok (refOpen.subtract (testG) == openSubtractG, "Range: refOpen.subtract(testG) == {}");
  t.ok (refOpen.subtract (testH) == openSubtractH, "Range: refOpen.subtract(testH) == {Range(refOpen.start,testH.start}");
  t.ok (refOpen.subtract (testI) == openSubtractI, "Range: refOpen.subtract(testI) == {Range(refOpen.start,testI.start}");

  // Adjacent ranges.
  Range left  (Datetime ("2016-04-25T11:00:00"), Datetime ("2016-04-25T12:00:00"));
  Range right (Datetime ("2016-04-25T12:00:00"), Datetime ("2016-04-25T13:00:00"));
  t.notok (left.overlap (right), "Range: left (11am - 12pm) does not overlap with right (12pm - 1pm)");

  auto intersection = left.intersect (right);
  t.ok (intersection.start.toEpoch () == 0, "Range: adjacent ranges do not intersect");
  t.ok (intersection.end.toEpoch ()   == 0, "Range: adjacent ranges do not intersect");

  // Range length.
  t.ok (Range (Datetime (2016, 1, 1), Datetime (2016, 1, 3)).total () == (2 * 86400),
       "Range (Datetime (2016, 1, 1), Datetime (2016, 1, 3)).total () == (2 * 86400)");

  return 0;
}

////////////////////////////////////////////////////////////////////////////////

