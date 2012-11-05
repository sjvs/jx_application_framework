<?php
// Do not edit this file directly!
// It is auto-generated by maven-php-plugin

// com.yahoo.satg.common.ui.tag.browser_code

function yt_core__versionJavaScript($params = array())
{
	return "YAHOO.SATG.appVersionInfo = {name:".(isset($params["name"])?$params["name"]:'').",value:".(isset($params["value"])?$params["value"]:'')."};";
}

function yt_core__crumbJavaScript($params = array())
{
	return "YAHOO.SATG.crumb={name:".(isset($params["name"])?$params["name"]:'').",value:".(isset($params["value"])?$params["value"]:'')."};";
}

function yt_core__com_yahoo_satg_common_ui_tag_profiler_code_min_js()
{
?>
<script type="text/javascript">
if(window.YAHOO==null)YAHOO={};YAHOO.tool={};YAHOO.tool.CallProfile=function(A,B){this.name=A;this.nestedCalls=[];this.end=function(D){if(D==null){var C=new Date();this.duration=C-this.start}else{this.duration=D}};this.codeBlock=false;this.start=B==null?new Date():B};YAHOO.tool.Profiler={_currentCall:null,_callStack:[],_callProfiles:[],_container:new Object(),_report:new Object(),_saveData:function(B,C){var A=this._report[B];A.calls++;A.points.push(C);if(A.calls>1){A.avg=((A.avg*(A.calls-1))+C)/A.calls;A.min=Math.min(A.min,C);A.max=Math.max(A.max,C)}else{A.avg=C;A.min=C;A.max=C}},getAverage:function(A){return this._report[A].avg},getCallCount:function(A){return this._report[A].calls},getMax:function(A){return this._report[A].max},getMin:function(A){return this._report[A].min},getFunctionReport:function(A){return this._report[A]},getFullReport:function(C){C=C||function(){return true};if(YAHOO.lang.isFunction(C)){var A={};for(var B in this._report){if(C(this._report[B])){A[B]=this._report[B]}}return A}},registerConstructor:function(B,A){this.registerFunction(B,A,true)},registerFunction:function(name,owner,registerPrototype){var funcName=(name.indexOf(".")>-1?name.substring(name.lastIndexOf(".")+1):name);if(!YAHOO.lang.isObject(owner)){owner=eval(name.substring(0,name.lastIndexOf(".")))}var method=owner[funcName];var prototype=method.prototype;if(YAHOO.lang.isFunction(method)&&!method.__yuiProfiled){this._container[name]=method;owner[funcName]=function(){YAHOO.tool.Profiler.startBlock(name,1);var start=new Date();var retval=method.apply(this,arguments);var stop=new Date();YAHOO.tool.Profiler.endBlock(stop-start);YAHOO.tool.Profiler._saveData(name,stop-start);return retval};YAHOO.lang.augmentObject(owner[funcName],method);owner[funcName].__yuiProfiled=true;owner[funcName].prototype=prototype;this._container[name].__yuiOwner=owner;this._container[name].__yuiFuncName=funcName;if(registerPrototype){this.registerObject(name+".prototype",prototype)}this._report[name]={calls:0,max:0,min:0,avg:0,points:[]}}return method},registerObject:function(name,object,recurse){object=(YAHOO.lang.isObject(object)?object:eval(name));this._container[name]=object;for(var prop in object){if(typeof object[prop]=="function"){if(prop!="constructor"&&prop!="superclass"){this.registerFunction(name+"."+prop,object)}}else{if(typeof object[prop]=="object"&&recurse){this.registerObject(name+"."+prop,object[prop],recurse)}}}},unregisterConstructor:function(A){if(YAHOO.lang.isFunction(this._container[A])){this.unregisterFunction(A,true)}},unregisterFunction:function(B,C){if(YAHOO.lang.isFunction(this._container[B])){if(C){this.unregisterObject(B+".prototype",this._container[B].prototype)}var A=this._container[B].__yuiOwner;var D=this._container[B].__yuiFuncName;delete this._container[B].__yuiOwner;delete this._container[B].__yuiFuncName;A[D]=this._container[B];delete this._container[B];delete this._report[B]}},unregisterObject:function(B,C){if(YAHOO.lang.isObject(this._container[B])){var A=this._container[B];for(var D in A){if(typeof A[D]=="function"){this.unregisterFunction(B+"."+D)}else{if(typeof A[D]=="object"&&C){this.unregisterObject(B+"."+D,C)}}}delete this._container[B]}},startBlock:function(C,B){var A=new YAHOO.tool.CallProfile(C,B);A.codeBlock=(B==null);if(this._callStack.length==0&&this._currentCall==null){this._callProfiles.push(A)}else{if(this._currentCall!=null){this._callStack.push(this._currentCall);this._currentCall.nestedCalls.push(A)}}this._currentCall=A},endBlock:function(A){this._currentCall.end(A);if(this._callStack.length>0){this._currentCall=this._callStack.pop()}else{this._currentCall=null}},printCallProfiles:function(){var A=window.open("","profileconsole");A.document.writeln('<html><head><title>Call Profile</title></head><body bgcolor="white" onLoad="self.focus()">');var C=[];if(this._callProfiles.length>0){C.push("<pre>");for(var B=0;B<this._callProfiles.length;B++){C.push("Call Profile : "+(B+1));C.push("------------------------");this._reportCall(this._callProfiles[B],C,[]);C.push("------------------------");C.push("")}C.push("</pre>")}else{C.push("No calls profiled")}A.document.writeln(C.join("\r\n"));A.document.writeln("</body></html>");A.document.close()},_reportCall:function(D,B,A){B.push(A.join("")+D.name+":"+D.duration);if(D.nestedCalls.length>0){A.push(" ");for(var C=0;C<D.nestedCalls.length;C++){this._reportCall(D.nestedCalls[C],B,A)}A.pop()}}};YAHOO={};YAHOO.tool={};YAHOO.tool.CallProfile=function(A,B){this.name=A;this.nestedCalls=[];this.end=function(D){if(D==null){var C=new Date();this.duration=C-this.start}else{this.duration=D}};this.codeBlock=false;this.start=B==null?new Date():B};YAHOO.tool.Profiler={_currentCall:null,_callStack:[],_callProfiles:[],_container:new Object(),_report:new Object(),_saveData:function(B,C){var A=this._report[B];A.calls++;A.points.push(C);if(A.calls>1){A.avg=((A.avg*(A.calls-1))+C)/A.calls;A.min=Math.min(A.min,C);A.max=Math.max(A.max,C)}else{A.avg=C;A.min=C;A.max=C}},getAverage:function(A){return this._report[A].avg},getCallCount:function(A){return this._report[A].calls},getMax:function(A){return this._report[A].max},getMin:function(A){return this._report[A].min},getFunctionReport:function(A){return this._report[A]},getFullReport:function(C){C=C||function(){return true};if(YAHOO.lang.isFunction(C)){var A={};for(var B in this._report){if(C(this._report[B])){A[B]=this._report[B]}}return A}},registerConstructor:function(B,A){this.registerFunction(B,A,true)},registerFunction:function(name,owner,registerPrototype){var funcName=(name.indexOf(".")>-1?name.substring(name.lastIndexOf(".")+1):name);if(!YAHOO.lang.isObject(owner)){owner=eval(name.substring(0,name.lastIndexOf(".")))}var method=owner[funcName];var prototype=method.prototype;if(YAHOO.lang.isFunction(method)&&!method.__yuiProfiled){this._container[name]=method;owner[funcName]=function(){YAHOO.tool.Profiler.startBlock(name,1);var start=new Date();var retval;try{retval=method.apply(this,arguments)}finally{var stop=new Date();YAHOO.tool.Profiler.endBlock(stop-start);YAHOO.tool.Profiler._saveData(name,stop-start);return retval}};YAHOO.lang.augmentObject(owner[funcName],method);owner[funcName].__yuiProfiled=true;owner[funcName].prototype=prototype;this._container[name].__yuiOwner=owner;this._container[name].__yuiFuncName=funcName;if(registerPrototype){this.registerObject(name+".prototype",prototype)}this._report[name]={calls:0,max:0,min:0,avg:0,points:[]}}return method},registerObject:function(name,object,recurse){object=(YAHOO.lang.isObject(object)?object:eval(name));this._container[name]=object;for(var prop in object){if(typeof object[prop]=="function"){if(prop!="constructor"&&prop!="superclass"){this.registerFunction(name+"."+prop,object)}}else{if(typeof object[prop]=="object"&&recurse){this.registerObject(name+"."+prop,object[prop],recurse)}}}},unregisterConstructor:function(A){if(YAHOO.lang.isFunction(this._container[A])){this.unregisterFunction(A,true)}},unregisterFunction:function(B,C){if(YAHOO.lang.isFunction(this._container[B])){if(C){this.unregisterObject(B+".prototype",this._container[B].prototype)}var A=this._container[B].__yuiOwner;var D=this._container[B].__yuiFuncName;delete this._container[B].__yuiOwner;delete this._container[B].__yuiFuncName;A[D]=this._container[B];delete this._container[B];delete this._report[B]}},unregisterObject:function(B,C){if(YAHOO.lang.isObject(this._container[B])){var A=this._container[B];for(var D in A){if(typeof A[D]=="function"){this.unregisterFunction(B+"."+D)}else{if(typeof A[D]=="object"&&C){this.unregisterObject(B+"."+D,C)}}}delete this._container[B]}},startBlock:function(C,B){var A=new YAHOO.tool.CallProfile(C,B);A.codeBlock=(B==null);if(this._callStack.length==0&&this._currentCall==null){this._callProfiles.push(A)}else{if(this._currentCall!=null){this._callStack.push(this._currentCall);this._currentCall.nestedCalls.push(A)}}this._currentCall=A},endBlock:function(A){if(this._currentCall!=null){this._currentCall.end(A);if(this._callStack.length>0){this._currentCall=this._callStack.pop()}else{this._currentCall=null}}},clearCallProfiles:function(){this._callProfiles=[];this._currentCall=null},printCallProfiles:function(){var A=window.open("","profileconsole");A.document.writeln('<html><head><title>Call Profile</title></head><body bgcolor="white" onLoad="self.focus()">');var C=[];if(this._callProfiles.length>0){C.push("<pre>");for(var B=0;B<this._callProfiles.length;B++){C.push("Call Profile : "+(B+1));C.push("------------------------");this._reportCall(this._callProfiles[B],C,[],1);C.push("------------------------");C.push("")}C.push("</pre>")}else{C.push("No calls profiled")}A.document.writeln(C.join("\r\n"));A.document.writeln("</body></html>");A.document.close()},_reportCall:function(D,B,A,E){B.push(A.join("")+D.name+":"+D.duration);if(D.nestedCalls.length>0&&E<5){A.push(" ");for(var C=0;C<D.nestedCalls.length;C++){this._reportCall(D.nestedCalls[C],B,A,E+1)}A.pop()}}};YAHOO={};YAHOO.tool={};YAHOO.tool.CallProfile=function(A,B){this.name=A;this.nestedCalls=[];this.end=function(D){if(D==null){var C=new Date();this.duration=C-this.start}else{this.duration=D}};this.codeBlock=false;this.start=B==null?new Date():B};YAHOO.tool.Profiler={_currentCall:null,_callStack:[],_callProfiles:[],_container:new Object(),_report:new Object(),_saveData:function(B,C){var A=this._report[B];A.calls++;A.points.push(C);if(A.calls>1){A.avg=((A.avg*(A.calls-1))+C)/A.calls;A.min=Math.min(A.min,C);A.max=Math.max(A.max,C)}else{A.avg=C;A.min=C;A.max=C}},getAverage:function(A){return this._report[A].avg},getCallCount:function(A){return this._report[A].calls},getMax:function(A){return this._report[A].max},getMin:function(A){return this._report[A].min},getFunctionReport:function(A){return this._report[A]},getFullReport:function(C){C=C||function(){return true};if(YAHOO.lang.isFunction(C)){var A={};for(var B in this._report){if(C(this._report[B])){A[B]=this._report[B]}}return A}},registerConstructor:function(B,A){this.registerFunction(B,A,true)},registerFunction:function(name,owner,registerPrototype){var funcName=(name.indexOf(".")>-1?name.substring(name.lastIndexOf(".")+1):name);if(!YAHOO.lang.isObject(owner)){owner=eval(name.substring(0,name.lastIndexOf(".")))}var method=owner[funcName];var prototype=method.prototype;if(YAHOO.lang.isFunction(method)&&!method.__yuiProfiled){this._container[name]=method;owner[funcName]=function(){YAHOO.tool.Profiler.startBlock(name,1);var start=new Date();var retval;try{retval=method.apply(this,arguments)}finally{var stop=new Date();YAHOO.tool.Profiler.endBlock(stop-start);YAHOO.tool.Profiler._saveData(name,stop-start);return retval}};YAHOO.lang.augmentObject(owner[funcName],method);owner[funcName].__yuiProfiled=true;owner[funcName].prototype=prototype;this._container[name].__yuiOwner=owner;this._container[name].__yuiFuncName=funcName;if(registerPrototype){this.registerObject(name+".prototype",prototype)}this._report[name]={calls:0,max:0,min:0,avg:0,points:[]}}return method},registerObject:function(name,object,recurse){object=(YAHOO.lang.isObject(object)?object:eval(name));this._container[name]=object;for(var prop in object){if(typeof object[prop]=="function"){if(prop!="constructor"&&prop!="superclass"){this.registerFunction(name+"."+prop,object)}}else{if(typeof object[prop]=="object"&&recurse){this.registerObject(name+"."+prop,object[prop],recurse)}}}},unregisterConstructor:function(A){if(YAHOO.lang.isFunction(this._container[A])){this.unregisterFunction(A,true)}},unregisterFunction:function(B,C){if(YAHOO.lang.isFunction(this._container[B])){if(C){this.unregisterObject(B+".prototype",this._container[B].prototype)}var A=this._container[B].__yuiOwner;var D=this._container[B].__yuiFuncName;delete this._container[B].__yuiOwner;delete this._container[B].__yuiFuncName;A[D]=this._container[B];delete this._container[B];delete this._report[B]}},unregisterObject:function(B,C){if(YAHOO.lang.isObject(this._container[B])){var A=this._container[B];for(var D in A){if(typeof A[D]=="function"){this.unregisterFunction(B+"."+D)}else{if(typeof A[D]=="object"&&C){this.unregisterObject(B+"."+D,C)}}}delete this._container[B]}},startBlock:function(C,B){var A=new YAHOO.tool.CallProfile(C,B);A.codeBlock=(B==null);if(this._callStack.length==0&&this._currentCall==null){this._callProfiles.push(A)}else{if(this._currentCall!=null){this._callStack.push(this._currentCall);this._currentCall.nestedCalls.push(A)}}this._currentCall=A},endBlock:function(A){if(this._currentCall!=null){this._currentCall.end(A);if(this._callStack.length>0){this._currentCall=this._callStack.pop()}else{this._currentCall=null}}},clearCallProfiles:function(){this._callProfiles=[];this._currentCall=null},printCallProfiles:function(E,B){this.depth=isNaN(E)?10:E;this.threshold=isNaN(B)?10:B;var A=window.open("","profileconsole");A.document.writeln('<html><head><title>Call Profile</title></head><body bgcolor="white" onLoad="self.focus()">');var D=[];if(this._callProfiles.length>0){D.push("<pre>");for(var C=0;C<this._callProfiles.length;C++){D.push("Call Profile : "+(C+1));D.push("------------------------");this._reportCall(this._callProfiles[C],D,[],1);D.push("------------------------");D.push("")}D.push("</pre>")}else{D.push("No calls profiled")}A.document.writeln(D.join("\r\n"));A.document.writeln("</body></html>");A.document.close()},_reportCall:function(D,B,A,E){if(this.threshold&&D.duration>this.threshold){B.push(A.join("")+D.name+":"+D.duration);if(D.nestedCalls.length>0&&E<this.depth){A.push(" ");for(var C=0;C<D.nestedCalls.length;C++){this._reportCall(D.nestedCalls[C],B,A,E+1)}A.pop()}}}};YAHOO={};YAHOO.tool={};YAHOO.tool.CallProfile=function(A,B){this.name=A;this.nestedCalls=[];this.end=function(D){if(D==null){var C=new Date();this.duration=C-this.start}else{this.duration=D}};this.codeBlock=false;this.start=B==null?new Date():B};YAHOO.tool.Profiler={_currentCall:null,_callStack:[],_callProfiles:[],_container:new Object(),_report:new Object(),_saveData:function(B,C){var A=this._report[B];A.calls++;A.points.push(C);if(A.calls>1){A.avg=((A.avg*(A.calls-1))+C)/A.calls;A.min=Math.min(A.min,C);A.max=Math.max(A.max,C)}else{A.avg=C;A.min=C;A.max=C}},getAverage:function(A){return this._report[A].avg},getCallCount:function(A){return this._report[A].calls},getMax:function(A){return this._report[A].max},getMin:function(A){return this._report[A].min},getFunctionReport:function(A){return this._report[A]},getFullReport:function(C){C=C||function(){return true};if(YAHOO.lang.isFunction(C)){var A={};for(var B in this._report){if(C(this._report[B])){A[B]=this._report[B]}}return A}},registerConstructor:function(B,A){this.registerFunction(B,A,true)},registerFunction:function(name,owner,registerPrototype){var funcName=(name.indexOf(".")>-1?name.substring(name.lastIndexOf(".")+1):name);if(!YAHOO.lang.isObject(owner)){owner=eval(name.substring(0,name.lastIndexOf(".")))}var method=owner[funcName];var prototype=method.prototype;if(YAHOO.lang.isFunction(method)&&!method.__yuiProfiled){this._container[name]=method;owner[funcName]=function(){YAHOO.tool.Profiler.startBlock(name,1);var start=new Date();var retval;try{retval=method.apply(this,arguments)}finally{var stop=new Date();YAHOO.tool.Profiler.endBlock(stop-start);YAHOO.tool.Profiler._saveData(name,stop-start);return retval}};YAHOO.lang.augmentObject(owner[funcName],method);owner[funcName].__yuiProfiled=true;owner[funcName].prototype=prototype;this._container[name].__yuiOwner=owner;this._container[name].__yuiFuncName=funcName;if(registerPrototype){this.registerObject(name+".prototype",prototype)}this._report[name]={calls:0,max:0,min:0,avg:0,points:[]}}return method},registerObject:function(name,object,recurse){object=(YAHOO.lang.isObject(object)?object:eval(name));this._container[name]=object;for(var prop in object){if(typeof object[prop]=="function"){if(prop!="constructor"&&prop!="superclass"){this.registerFunction(name+"."+prop,object)}}else{if(typeof object[prop]=="object"&&recurse){this.registerObject(name+"."+prop,object[prop],recurse)}}}},unregisterConstructor:function(A){if(YAHOO.lang.isFunction(this._container[A])){this.unregisterFunction(A,true)}},unregisterFunction:function(B,C){if(YAHOO.lang.isFunction(this._container[B])){if(C){this.unregisterObject(B+".prototype",this._container[B].prototype)}var A=this._container[B].__yuiOwner;var D=this._container[B].__yuiFuncName;delete this._container[B].__yuiOwner;delete this._container[B].__yuiFuncName;A[D]=this._container[B];delete this._container[B];delete this._report[B]}},unregisterObject:function(B,C){if(YAHOO.lang.isObject(this._container[B])){var A=this._container[B];for(var D in A){if(typeof A[D]=="function"){this.unregisterFunction(B+"."+D)}else{if(typeof A[D]=="object"&&C){this.unregisterObject(B+"."+D,C)}}}delete this._container[B]}},startBlock:function(C,B){var A=new YAHOO.tool.CallProfile(C,B);A.codeBlock=(B==null);if(this._callStack.length==0&&this._currentCall==null){this._callProfiles.push(A)}else{if(this._currentCall!=null){this._callStack.push(this._currentCall);this._currentCall.nestedCalls.push(A)}}this._currentCall=A},endBlock:function(A){if(this._currentCall!=null){this._currentCall.end(A);if(this._callStack.length>0){this._currentCall=this._callStack.pop()}else{this._currentCall=null}}},clearCallProfiles:function(){this._callProfiles=[];this._currentCall=null},printCallProfiles:function(E,B){this.depth=isNaN(E)?10:E;this.threshold=isNaN(B)?10:B;var A=window.open("","profileconsole");A.document.writeln('<html><head><title>Call Profile</title></head><body bgcolor="white" onLoad="self.focus()">');var D=[];if(this._callProfiles.length>0){D.push("<pre>");D.push("Min Threshold (ms): "+this.threshold);D.push("Max Stack Depth   : "+this.depth);D.push("------------------------");for(var C=0;C<this._callProfiles.length;C++){if(this.threshold&&this._callProfiles[C].duration>this.threshold){D.push("Call Profile : "+(C+1));D.push("------------------------");this._reportCall(this._callProfiles[C],D,[],1);D.push("------------------------");D.push("")}}D.push("</pre>")}else{D.push("No calls profiled")}A.document.writeln(D.join("\r\n"));A.document.writeln("</body></html>");A.document.close()},_reportCall:function(D,B,A,E){if(this.threshold&&D.duration>this.threshold){B.push(A.join("")+D.name+":"+D.duration);if(D.nestedCalls.length>0&&E<this.depth){A.push(" ");for(var C=0;C<D.nestedCalls.length;C++){this._reportCall(D.nestedCalls[C],B,A,E+1)}A.pop()}}}};</script>

<?php
function yt_core__debugJavaScript($params = array())
{
	return "if (YAHOO.SATG.Debug) YAHOO.SATG.Debug.debug_output_div=".(isset($params["div"])?$params["div"]:'').";";
}
?>