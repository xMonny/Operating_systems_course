#!/bin/bash

cat ~/newPass | awk -F : 'BEGIN { printf ("<table>\n\t<tr>\n\t  <td>%s</td>\n\t  <td>%s</td>\n\t  <td>%s</td>\n\t  <td>%s</td>\n\t</tr>\n", "Username", "Group", "Login Shell", "GECKO") } { printf ("\t<tr>\n\t  <td>%s</td>\n\t  <td>%s</td>\n\t  <td>%s</td>\n\t  <td>%s</td>\n\t</tr>\n", $1, $4, $5, $7) } END { printf ("</table>\n") }'
