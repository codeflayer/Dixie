//
// Dixie
// Copyright 2015 Skyscanner Limited
//
// Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License.
// You may obtain a copy of the License at:
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and limitations under the License.

#import "DixieBaseChaosProvider.h"
#import "DixieCompositeCondition.h"

/**
 *  Provides a behaviour, where the method's implementation can be changed according the passed arguments
 */
@interface DixieCompositeChaosProvider : DixieBaseChaosProvider

/**
 *  Creates a new instance of DixieCompositeChaosProvider
 *
 *  @param arrayOfConditions an array of DixieCompositeConditions
 *
 *  @return a new instance of DixieCompositeChaosProvider
 */
+(instancetype) conditions:(NSArray*)arrayOfConditions;

@end