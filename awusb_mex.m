% awusb_mex() - mex interface to the awusb-linux library by Bryan Kate
%               (required)
%
% Usage:
%   >> awusb_mex(command)
%   >> portvalue = awusb_mex(command, device)
%   >> awusb_mex(command, device, portvalue)
%
% Inputs:
%   command   - string 'Open', 'Close', 'Toggle', 'Read', or 'Write'
%   device    - scalar double device number
%   portvalue - 1x16 double vector pin state for 'Write' (0: low; 1: high)
%               or 'Toggle' (0: input; 1; output)
%
% Outputs:
%   portvalue - 1x16 double vector pin state for 'Read' (0: low; 1: high)
%
% Examples:
%   awusb_mex('Open', 1);
%   awusb_mex('Toggle', 1, zeros(1, 16));
%   portvalue = awusb_mex('Read', 1);
%   awusb_mex('Toggle', 1, ones(1, 16));
%   awusb_mex('Write', ones(1, 16));
%   awusb_mex('Close', 1);
%   awusb_mex('CloseAll');
%   clear awusb_mex
%
% Note:
%   Requires awusb-linux (download from
%   http://sourceforge.net/projects/awusb-linux/) and libusb-dev. Patch
%   awusb.c, with awusb-linux-2.0.patch before compilation (with mex
%   awusb_mex.c -lusb). ActiveWire firmware must be loaded. Usage of udev
%   rules is recommendended to load firmware at boot time (see provided
%   example file for Ubuntu). Devices should be plugged in before first use
%   of awusb_mex. In case a device is plugged later a rescan of the USB bus
%   can be forced with clear awusb_mex
%
% Author: Andreas Widmann, University of Leipzig, 2012

%123456789012345678901234567890123456789012345678901234567890123456789012

% Copyright (C) 2012 Andreas Widmann, University of Leipzig, widmann@uni-leipzig.de
%
% This program is free software; you can redistribute it and/or modify it
% under the terms of the GNU General Public License as published by the
% Free Software Foundation; either version 2 of the License, or (at your
% option) any later version.
%
% This program is distributed in the hope that it will be useful, but
% WITHOUT ANY WARRANTY; without even the implied warranty of
% MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General
% Public License for more details.
%
% You should have received a copy of the GNU General Public License along
% with this program; if not, write to the Free Software Foundation, Inc.,
% 59 Temple Place, Suite 330, Boston, MA  02111-1307, USA

function [ portvalue ] = awusb_mex(command, device, portvalue)
