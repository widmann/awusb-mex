% awusb_demo() - Demo how to use awusb_mex for response registration
%
% Usage:
%   >> awusb_demo
%
% Note:
%   Button 1-3 presses and releases are displayed. Use button 4 to exit.
%
% Author: Andreas Widmann, University of Leipzig, 2016

%123456789012345678901234567890123456789012345678901234567890123456789012

% Copyright (C) 2016 Andreas Widmann, University of Leipzig, widmann@uni-leipzig.de
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

function awusb_demo

% Defaults
more off;
stringArray = { 'released', 'pressed' };
awusb_state = zeros( 1, 16 );

% Open ActiveWire device
awusb_mex( 'Open', 1 );

% Switch to input
awusb_mex( 'Toggle', 1, zeros( 1, 16 ) );

while awusb_state( 4 ) ~= 1
    
    % Read data
    portvalue = awusb_mex( 'Read', 1 );
    
    if any( portvalue( 1:4 ) ~= awusb_state( 1:4 ) )

        button = find( portvalue( 1:4 ) ~= awusb_state( 1:4 ), 1 );
        fprintf( 'Button %d %s.\n', button, stringArray{ portvalue( button ) + 1 } )
        awusb_state = portvalue;   

    end
    
    WaitSecs( 'YieldSecs', 0.001 );
        
end

% Close ActiveWire device
awusb_mex('Close', 1);

end
