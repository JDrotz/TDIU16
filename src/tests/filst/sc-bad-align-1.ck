# -*- perl -*-
use strict;
use warnings;
use tests::tests;
check_expected ([<<'EOF']);
(sc-bad-align) begin
sc-bad-align: exit(-1)
EOF
pass;
