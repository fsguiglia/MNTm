<p align="justify">
<b> El mapa no es el territorio </b> (MNT) is a set of open source tools that facilitates the design of visual and musical mappings for interactive installations and performances. These tools are presented as easily understood spatial metaphors, enabling artists to experiment with mapping in the collaborative design of interactive environments for multidisciplinary projects. 
</p>
<span>&#8226;</span> More info on <a href="http://www.nime.org/proceedings/2019/nime2019_paper029.pdf"> Fabián Sguiglia, Pauli Coton, and Fernando Toth. 2019. El mapa no es el territorio: Sensor mapping for audiovisual performances. <i> Proceedings of the International Conference on New Interfaces for Musical Expression </i>, UFRGS, pp. 146–149.</a>

<BR>
<h1>MNTm</h1>
<p> openFrameworks 0.10.1. Tested in windows 10. See <a href= "https://github.com/fsguiglia/MNTm4l"> MNTm4l </a> to use MNT with Ableton Live. </p>
<p> Windows binary: www.sguiglia.com.ar/soft/MNTm.zip (requires <a href="https://support.microsoft.com/en-us/help/2977003/the-latest-supported-visual-c-downloads"> Microsoft Visual C++ Redistributable for VS 2017 </a>). </p>
<BR>
<p  ALIGN="JUSTIFY">MNTm receives OSC  messages or JSON formated messages via UDP to ports 21235 and 21234   respectively. If you are using another MNT app for sensing,  communication shouldn't be an issue. Otherwise, for OSC  communication, a single message containing a list of floats should be sent for every player or detected object. If using JSON, the messages should be formated as an array of objects, where every object represents a player or detected object, and contains a list of  features, including an unique ID.</p>
<p><BR>
</p>
<p><strong>JSON:</strong> [{ id: 01, x: 121, y: 200, v: 0.1},{id: 02, x: 20, y: 83, v: 0.8}]</p>
<p><strong>OSC: </strong>[1.0, 121.0, 200.0, 0.1], [2.0, 20.0, 83.0, 0.8]</p>
<p><BR>
</p>
<h2>Menu</h2>
<p>Use the top menu to browse module tabs.</p>
<p><strong>Save: </strong>save project.</p>
<p><strong>Load: </strong>load project.</p>
<p><strong>Mouse in: </strong>use mouse as input, generates 'x' and 'y' keys.</p>
<p><strong>Add dummy: </strong>use one ore more brownian motion generators as input, useful to test  project without sensor input. Generates 'x' and 'y' keys. </p>
<p><strong>Clear dummies: </strong>remove  brownian motion generators.</p>
<p><BR>
</p>
<h2>Shortcuts</h2>
<p><strong>Tab: </strong>browse module tabs.</p>
<p><strong>Page Up / Page Down</strong>:  scroll components.</p>
<p><BR>
</p>
<h2><strong>Modules</strong></h2>
<p>Incoming data is first  processed in &lsquo;Preprocessor&rsquo;. This module&rsquo;s output is fed to a  bus. The output of the remaining modules can provide input to any of the following modules.</p>
<p><BR>
</p>
<h3>Preprocessor</h3>
<p>Click add to add a feature. 'Key' should match one of the names in the JSON  object or the numeric index of the desired feature when using OSC. If data is  received from another MNT app, check it's help to see available features.</p>
<p><strong>Normalize:</strong> toggle feature normalization (minmax normalization).</p>
<p><strong>Norm:</strong> select input and output range for normalization. For instance '100,200,0,1' (without quotes) will map values between 100 and 200 to a range between 0 and 1</p>
<p><strong>MinMax:</strong> generate a new key containing the minimum and maximum values for the  selected feature. For instance if the selected feature is 'x', this  option will generate two new keys: 'xMin' and 'xMax'.</p>
<p><strong>Average: </strong>generate  a new key containing the average value for the selected feature. For  instance if the selected feature is 'x', this option will generate a  new keys: 'xAv' (without quotes).</p>
<p><strong>Roi: </strong>Discard  objects with values outside the range selected in 'Norm'</p>
<p><strong>Draw:</strong> draw the values</p>
<p><strong>Print: </strong>print  the values</p>
<p><strong>Remove: </strong>remove  this component</p>
<BR>
<h3>NNI</h3>
<p>Interpolate  presets by drawing zones. <BR>
</p>
<p>Click 'add' to add a new NNI component. Data produced by this component is  accessible from the following modules by selecting the feature:  nni + id + &ldquo;-&rdquo; + zone (id 0, zone 1 would be nni0-1).</p>
<p><strong>Keys: </strong>a  key pair for x/y input, for instance 'xAv,yAv' (without quotes).</p>
<p><strong>Address: </strong>OSC  address to send results. This component will send zone weights for  every received object containing the selected features. If you want a  single weight per zone you should use min, max or average values (see  Preprocessor). If you are using MNT in Max for live you don't have to  worry about this.</p>
<p><strong>Show: </strong>show this component's display (useful when using multiple NNI components).</p>
<p><strong>Active: </strong>activate  this component.</p>
<p><strong>Clear: </strong>clear  zones.</p>
<p><strong>Remove:</strong> remove this component</p>
<p><strong>Display: </strong>click  and drag when inactive to create and place new zones. You can't  delete or rearrenge existing zones. I'm working on it.</p>
<BR>
<h3>RGB</h3>
<p>Interpolate  presets using colors in a loaded image.</p>
<p>Click  add to add a new RGB component. Data produced by this component is  accessible from the following modules by selecting the feature:  rgb + id + &ldquo;-&rdquo; + channel (id 0, red channel would be rgb0-r).<BR>
</p>
<p><strong>Keys: </strong>a  key pair for x/y input, for instance 'xAv,yAv' (without quotes). </p>
<p><strong>Address: </strong>OSC  address to send results. This component will send RGB information for  every received object containing the selected features. If you want a  single value per channel you should min, max or average values (see  Preprocessor). If you are using MNT in Max for live you don't have to  worry about this.</p>
<p><strong>Show: </strong>show this component's display (useful when using multiple RGB components).</p>
<p><strong>Active:</strong> activate this component.</p>
<p><strong>Load: </strong>load  an image (it will be resized to 400,400).</p>
<p><strong>Remove</strong>:  remove this component.</p>
<p><strong>Size:</strong> window size.</p>
<BR>
<h3>Trigger</h3>
<p>Trigger  conditional events.</p>
<p>Write  the number of keys the trigger should watch to add a trigger. For  instance if only the position in x is relevant, you should write '1' (without quotes). Key, thresh, margin and invert must be set for every key.</p>
<p><strong>Key: </strong>a  single key, for instance 'x' (without quotes).</p>
<p><strong>Thresh: </strong>threshold. For instance if you need to  trigger an event when a performer is in the right side of the stage,  key would be 'x' and thresh would be '0.5,1' (between 0,5 and 1,  without quotes).</p>
<p><strong>Margin: </strong>a  margin to avoid noisy signals to constantly turn on and off the  trigger. If thresh is '0.5,1' and margin is '0.1' the trigger will  turn on when values are between 0.5 ad 1, but it will turn off when  they are above 1.1 or below 0.4. </p>
<p><strong>Invert: </strong>Invert  the trigger. Whith thresh '0.5,1' the trigger will turn on below 0.5 or above 1. </p>
<p><strong>Address: </strong>OSC  address to send results. 0 or 1 will be sent for trigger on/off.</p>
<p><strong>Active:</strong> activate this component.</p>
<p><strong>Remove:</strong> remove this component.</p>
<BR>
<h3>Maps</h3>
<p>Scale  and send values. </p>
<p>Write  the number of keys to add a component. The keys will be grouped and sent in a single OSC message.</p>
<p><strong>Key: </strong>a  single key, for instance 'x' (without quotes).</p>
<p><strong>Range: </strong>output  range (assumes input range from 0 to 1)</p>
<p><strong>Exp: </strong>exponent (create different curves)</p>
<p><strong>Crop: </strong>limit  values to range</p>
<p><strong>Address:</strong> OSC address to send results</p>
<p><strong>Active:</strong> activate this component.</p>
<p><strong>Remove:</strong> remove this component.</p>
<BR>
<h3>OSC</h3>
<p>Set  multiple  ip/port pairs in  order to communicate MNTm with other software</p>
<p>Click  add to add a component.</p>
<p><strong>Ip:port: </strong>set  ip/port pair, for instance '127.0.0.01:7000'  (without quotes).</p>
<p><strong>Remove:</strong> remove this component.</p>
