/*
  Copyright 2008-2009 Larry Gritz and the other authors and contributors.
  All Rights Reserved.

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions are
  met:
  * Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright
    notice, this list of conditions and the following disclaimer in the
    documentation and/or other materials provided with the distribution.
  * Neither the name of the software's owners nor the names of its
    contributors may be used to endorse or promote products derived from
    this software without specific prior written permission.
  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

  (This is the Modified BSD License)
*/

#include "fits_pvt.h"

namespace fits_pvt {



std::string num2str (float val)
{
    std::stringstream out ;
    out << val;
    std::string result (20 - out.str().size(), ' ');
    result += out.str ();
    return result;
}



std::string create_card (std::string keyname, std::string value,
                         std::string comment)
{
    if (keyname.substr (0, 7) == "COMMENT" || keyname.substr (0, 7) == "HISTORY")
        keyname = keyname.substr (0, 7);
    else if (keyname.substr (0, 8) == "HIERARCH")
        keyname = "HIERARCH";

    std::string card = keyname;
    card.resize (8, ' ');
    if (keyname != "COMMENT" && keyname != "HISTORY" && keyname != "HIERARCH")
        card += "= ";
    if (value.size ())
        card += value;
    if (comment.size ())
        card += " / " + comment;
    card.resize (80, ' ');
    return card;
}



void
unpack_card (const std::string &card, std::string &keyname, std::string &value,
             std::string &comment)
{
    keyname.clear ();
    value.clear ();
    comment.clear ();

    // extracting keyname - first 8 bytes of the keyword (always)
    // we strip spaces that are placed after keyword name
    keyname = pystring::strip (card.substr(0,8));

    // the value starts at 10 byte of the card if "=" is present at 8 byte
    // or at 8 byte otherwise
    int start = 10;
    if (card[8] != '=')
        start = 8;
    // copy of the card with keyword name stripped (only value and comment)
    std::string card_cpy = card.substr (start, card.size ());
    card_cpy = pystring::strip (card_cpy, "");

    // card value may be simple character string enclosed in a single quote
    // in that case we have to copy value of the string (with quotes) by hand
    // to prevent splitting card when the value contains multiple / signs
    int end_str = 0; // end of the character string
    if (card_cpy[0] == '\'') {
        end_str = pystring::find (card_cpy, "'", 1, card_cpy.size ());
        value += card_cpy.substr (0, end_str);
    }

    // splitting value and comment
    std::vector<std::string> result;
    std::string sep("/");
    pystring::split (card_cpy.substr (end_str, card_cpy.size ()), result,
                     sep, 1);

    // result[0] - value of the card
    // result[1] - comment fot the value
    if (result[0].size() > 0)
        value += pystring::strip (result[0], "");
    if (result.size () > 1 && result[1].size() > 0)
        comment = pystring::strip (result[1], "");

}

} // namespace fits_pvt
