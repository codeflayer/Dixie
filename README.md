Dixie
===
<img src="/Logo.png?raw=true" alt="Dixie" width="100px" height="auto">

Dixie is an open source Objective-C testing framework for altering object behaviours. Test your app through creating chaos in the inner systems. The primary goal of Dixie is to provide a set of tools, which the developers can test their code with. Behind the goal is the ideology of _"do not always expect the best"_. You can read more about this [here](https://medium.com/@Skyscanner/dixie-turning-chaos-to-your-advantage-4f3749e6d485).

[![Build Status](https://travis-ci.org/Skyscanner/Dixie.svg)](https://travis-ci.org/Skyscanner/Dixie)

### How can you test your application with Dixie
1. Create a new target in your app’s project that runs your Dixie setup logic during app launch. With the separate target you can make sure all Dixie related code is separated and won’t be included in your production builds
2. Change the behaviour of some components with Dixie and deploy the test build to device or simulator to see how your app behaves
3. Once you got familiar with the library, it might be worth to create a list of behaviour changes that you can easily configure and combine from your debug build

### A few ideas what you can do
* Hijack the localisation component of your app to simulate long strings or other unexpected text
* Inject mocked network responses into the network layer (you can match the URLs with a regular expression so you can provide different responses for different requests)
* Network mocking can also be utilised in automated UI tests, so you don’t have to rely on real network communication
* You can easily mock GPS coordinates or even the current date, so you do not have to set the simulators location manually
* Inject randomised properties to your data models to see how robust is your application handling the objects received from web

### In our projects we use Dixie in two ways

1. We just put the Dixie configuration code in the `AppDelegate` and remove if not needed (these are short testing sessions). `#ifdef`-ing is an option, also creating a separate target that has a category on the `AppDelegate`.
2. We use Dixie in the automated UI tests as a standard mocking framework. The tests rely on the final app target, and we found extremely hard to mock components on the low level (e.g. networking) in this scope.

Either way we think Dixie comes handy in cases, where you have to mock libraries, that's less configurable or some components are not that easily injectable.



## Installation
#### With CocoaPods
[CocoaPods](https://cocoapods.org) is the recommended way to add Dixie to your project.

- Add Dixie to your Podfile

	`pod 'Dixie'`
- Install/update pod(s)

	`pod install`

- Include DixieHeaders.h where you would like to use Dixie

	`#import <DixieHeaders.h>`

#### Without CocoaPods
You can add Dixie without CocoaPods to your project if you download the Dixie project and add it manually to your project. Don't forget to add the project path into the Header Search Path.

You can see an example for this integration in the [Example app](https://github.com/Skyscanner/Dixie#example-app)




##Usage
First define which method on which class the change should be applied to, and its new behaviour. You can do this by creating a `DixieProfileEntry`:

```objective-c
//Tomorrow
NSDate* testDate = [NSDate dateWithTimeIntervalSinceNow:24*60*60];

//A behaviour to always return tomorrow's date
DixieChaosProvider* provider = [DixieConstantChaosProvider constant:testDate];

//Create the entry
DixieProfileEntry* entry = [DixieProfileEntry entry:[NSDate class] selector:@selector(date) chaosProvider:provider]
```
	
Then create an instance of a `Dixie` configuration, set the profile and apply.

```objective-c
//Create Dixie configuration
Dixie* dixie = [Dixie new];
	
//Set and apply change
dixie
	.Profile(entry)
	.Apply();
```

After applying the profile, every call of `[NSDate date]` will return the date for tomorrow instead of today. This way you can test date issues without going to the device settings and changing the date manually.

When you no longer need Dixie, revert your change:

```objective-c
//Revert the change of the entry
dixie
	.RevertIt(entry);
```

Full code:

```objective-c
- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions
{
	NSDate* testDate = [NSDate dateWithTimeIntervalSinceNow:24*60*60];

	DixieChaosProvider* provider = [DixieConstantChaosProvider constant:testDate];
	
	DixieProfileEntry* entry = [DixieProfileEntry entry:[NSDate class] selector:@selector(date) chaosProvider:provider]
		
	Dixie* dixie = [Dixie new];

	dixie
		.Profile(entry)
		.Apply();
	
	return YES;	
}
```

You can set multiple profiles and also revert them all at once. You can also choose from some preset behaviours:

####DixieNonChaosProvider
Provides the original behaviour. Good to use when you want to have a different behaviour in special cases only.

####DixieConstantChaosProvider
Provides a behaviour that always returns a constant object.

####DixieNilChaosProvider
Provides a behaviour that always returns `nil`.

####DixieBlockChaosProvider
Provides a behaviour that is described by a block. Using this provider the method can be replaced with a full custom behaviour. For accessing method parameters and setting the return value you can use the `DixieCallEnvironment` object passed to the block.

####DixieRandomChaosProvider
Provides a behaviour that returns a random object. The default implementation returns a random `NSNumber`.

####DixieExceptionChaosProvider
Provides a behaviour that throws an exception.

####DixieSequentialChaosProvider
For every call it returns the _ith_ chaosprovider's behaviour, where `i` is the number of the call. If the number of calls exceeds the number of predefined chaosprovider the last provider's behaviour will be used.

####DixieCompositeChaosProvider
Checks the parameters of the method and if one matches the value of a given `DixieCompositeCondition`, then it returns the connected chaosprovider's behaviour.

# Under the hood
The idea of changing an object's behaviour is not new. It is usually used in unit testing, where a component's dependencies are mocked to have a controlled, reproducible environment. In these situations there is the requirement that the target project should be _easily injectable_. If you are depending on components that are not made by you, or that are not injectable, you have to turn to different methods. To implement the theory of creating chaos/altering component behaviour in Objective-C environment, Dixie uses the technique of _Method Swizzling_. Method swizzling relies on calling special runtime methods, that require knowing the target method and its environment. Dixie takes care of handling the runtime for you, and also hides the original method environment, so you only have to focus on defining the new behaviour and can apply it quickly and simply.

__Note:__ 
* The current implementation is best at changing behaviours of methods on iOS simulator. Support for arm architectures will come in the next version.
* Dixie is best for testing so, as with other similar libraries, its usage in production environments is strongly discouraged.


# Example app
You can find a [Dixie example app project](https://github.com/Skyscanner/Dixie/tree/master/DixieExampleApp) in the repository with some common use-cases of how to use Dixie. The project requires [CocoaPods](https://cocoapods.org) dependency manager, so you have to run the `pod install` command in the `DixieExampleApp` directory before you can run the project.

The example app covers three use-cases:

#### Location mocking
Shows the actual location on a map using the [`CLLocationManager`](https://developer.apple.com/library/ios/documentation/CoreLocation/Reference/CLLocationManager_Class/index.html). Dixie changes the implementation of the [`locationManager:didUpdateLocations:`](https://developer.apple.com/library/ios/documentation/CoreLocation/Reference/CLLocationManagerDelegate_Protocol/#//apple_ref/occ/intfm/CLLocationManagerDelegate/locationManager:didUpdateLocations:) method, so any location can be mocked easily. The example app mocks a random city. With Dixie Revert function the device location is used. The whole logic can be found in the [`MapViewController.m`](https://github.com/Skyscanner/Dixie/blob/master/DixieExampleApp/DixieExampleApp/MapViewController.m). It uses a `DixieBlockChaosProvider` to be able to change the method implementation with a block.

#### Date mocking
A countdown timer to the next [Halley's Comet](http://en.wikipedia.org/wiki/Halley's_Comet) arrival. The countdown timer uses the actual date function ([`[NSDate date]`](https://developer.apple.com/library/ios/documentation/Cocoa/Reference/Foundation/Classes/NSDate_Class/#//apple_ref/occ/clm/NSDate/date)) and Dixie changes the implementation of this method and mocks a random date between -10000 and +10000 days from the actual date. The whole logic can be found in the [`CountDownViewController.m`](https://github.com/Skyscanner/Dixie/blob/master/DixieExampleApp/DixieExampleApp/CountDownViewController.m). It uses a `DixieConstantChaosProvider` which provides a constant value mocking.

#### Network mocking
This example shows the weather at the actual location using [OpenWeatherMap API](http://openweathermap.org/api) as the data source and [AFNetworking](https://github.com/AFNetworking/AFNetworking) which is a popular iOS and OS X networking framework. Dixie changes the implementation of the `GET:parameters:success:failure:` method implementation of the [`AFHTTPRequestOperationManager`](https://github.com/AFNetworking/AFNetworking/blob/7f997ef99ae64e321b6747defcaae5b13a691119/AFNetworking/AFHTTPRequestOperationManager.h) class of the `AFNetworking` framework. The request is not going out to the network, Dixie creates the response object and calls the `success` callback which is the async callback coming from a successful network response. The whole logic can be found in the [`WeatherViewController.m`](https://github.com/Skyscanner/Dixie/blob/master/DixieExampleApp/DixieExampleApp/WeatherViewController.m), it uses a `DixieBlockChaosProvider`.


#About
The Dixie was born from the idea of Peter Adam Wiesner. The prototype was brought to life by Phillip Wheatley, Tamas Flamich, Zsolt Varnai and Peter Adam Wiesner within a research lab project in one week. The prototype was developed into this open source library by Zsolt Varnai, Csaba Szabo, Zsombor Fuszenecker and Peter Adam Wiesner.

If you know a way to make Dixie better, please contribute!

You can reach us:

* [peter.wiesner@skyscanner.net](peter.wiesner@skyscanner.net) or  [@Peteee24](https://twitter.com/peteee24)
* [zsolt.varnai@skyscanner.net](zsolt.varnai@skyscanner.net)
* [csaba.szabo@skyscanner.net](csaba.szabo@skyscanner.net)
* [zsombor.fuszenecker@skyscanner.net](zsombor.fuszenecker@skyscanner.net)
