//
// @file ydk_yang.hpp
// @brief The ydk model.
//
// YANG Development Kit
// Copyright 2016 Cisco Systems. All rights reserved
//
////////////////////////////////////////////////////////////////
// Licensed to the Apache Software Foundation (ASF) under one
// or more contributor license agreements.  See the NOTICE file
// distributed with this work for additional information
// regarding copyright ownership.  The ASF licenses this file
// to you under the Apache License, Version 2.0 (the
// "License"); you may not use this file except in compliance
// with the License.  You may obtain a copy of the License at
//
//   http://www.apache.org/licenses/LICENSE-2.0
//
//  Unless required by applicable law or agreed to in writing,
// software distributed under the License is distributed on an
// "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
// KIND, either express or implied.  See the License for the
// specific language governing permissions and limitations
// under the License.
//
//////////////////////////////////////////////////////////////////

#include "ydk_yang.hpp"

namespace ydk {
    namespace path {
    const char* YDK_MODULE_NAME = "ydk";
    const char* YDK_MODULE_REVISION = "2016-02-26";
    const char* YDK_MODULE = R"(

    module ydk {
      namespace "http://cisco.com/ns/yang/ydk";
      prefix "ydk";
      organization "Cisco Systems, Inc.";
      contact
        "Cisco Systems, Inc.
         Customer Service
         Postal: 170 West Tasman Drive
         San Jose, CA 95134
         Tel: +1 800 553-NETS
         E-mail: cs-yang@cisco.com";
      description 
        "This module defines  the core YDK model.  
        Top level objects of this model include
        + modules --> All the modules referenced by the Bundles. This is a 
                      repository of all YANG modules/submodules
        + service-providers --> All the registered service providers we know of
        						A service provider should augment the list here.
        						Implementations once active can show them selves here.
        + bundles --> All bundles that comprise the runtime
        			  A bundle is a collection YANG modules that are converted to an API
        			  in a target language/environment. Each bundle is defined in a file that is encoded in JSON/XML
        			  based on this YANG model. It lists among other things the data models that constitute this bundle
        			  as well as all the dependencies this bundle has. ydk-gen can use a bundle description today to 
        			  produce a pip-installable package for the python API generated from these models.
        			  In the future the same mechanism can be used to create shared libraries , java jar files etc..
                      from a bundle profile.     
        
        ";

      revision "2016-02-26" {
        description
          "Initial revision.";
      }
      typedef VERSION-NUMBER {
      	type string {
      	  pattern "[0-9][0-9]";
      	 }
      }
      typedef revision-identifier {
      	type string {
      		pattern '\d{4}-\d{2}-\d{2}';
      	}
      	description "Represents a specific date in YYYY-MM-DD format.";
      }
      extension type-constraint {
      	 description "This extension is used under an instance-identifier type. It is used to 
      	 qualify the instance-identifier with the type of target node identified by the path.
      	 
      	 A note a given type instance-identifier can have multiple targets
      	 ";
         argument path;
      }
      extension service-provider-class-name {
      	description "The service provider class name extension. Service Providers
      	are modelled in yang as augment statemnts that augment /ydk:service-providers/ydk:service-provider.
      	This extension is a must to determine the name of the service-provider-class.";
      	argument name;
      }
      extension service-class-name {
      	description "The default behavior of ydk-gen is to group all rpc statements
      	defined in a module and create a service class of the form <module-name>Service
      	which will have methods that correspond to each rpc. The service class is
      	a wrapper around the RPC classes that makes it easier to consume the RPCs defined
      	in a module. This extension can be used to create associate an rpc with a 
      	ServiceClass that will be generated as part of this module's transformation'";
      	argument name;
      }
      grouping semantic-version {
      	description "The semantic version of the form major.minor.patch-level.suffix";
      			
      	leaf major-version-number {
    		config false;
      		type VERSION-NUMBER;
      	}	
      	leaf minor-version-number {
      		config false;
      		type VERSION-NUMBER;
      	}	
      	leaf patch-level {
    		type VERSION-NUMBER;
      	}	
      	leaf suffix {	
      		type string;
      	}
      }
      
    container ydk { 
      container modules {
      	config false;
      	description "Model of the YANG statements represented by this module or submodule .
      		A statement consists of a keyword followed by an argument and
      		has list of substatements . Each statement is identified by a key
      		that refers to row and column number in the file.";
      	list module {
      		key "name revision";
      		description "Each entry represents one module/submodule";
      		
      		leaf name {
    			type string;
    			description "The YANG module or submodule name.";
    		}
    		
    		leaf revision {
    			type union {
    				type revision-identifier;
    				type string { length 0; }
    			}
    			description
    				"The YANG module or submodule revision date.
    				A zero-length string is used if no revision statement
    				is present in the YANG module or submodule.";
    		}
    			
      		leaf uri {
      			type string;
      			description 
      				"Contains a URL that represents the YANG schema 
      				resource for this module or submodule.
      				
      				This leaf will only be present if there is a URL 
      				available for retrieval of the schema for this entry.";
      		}
      			
      		leaf kind {
    			type enumeration {
    		  		enum MODULE {
    		  			value 0;
    		  		}
    		  
    		  		enum SUBMODULE {
    		     			value 1;
    		  		}
    			}
    		}
      			
      		container statements {
      			list statement {
      			    description "Represents a statement in a YANG model. Statement represents
      			    		a meta-model of the YANG language itself. Any YANG document is composed of 
      			    		statements that are of the form keyword <optional-arg> followed by a 
      			    		semi colon or a block { <with zero or more statements> }
      			    
      			    				Because this represents a list of statements there is a need to uniquely identify
      			    				each statement
      			    
      			    				";
      				key id;
      				
      				leaf id {
      					description "A statement is defined by its keyword which is a tuple of the form
       						((name of the module, keyword), arg).Note the name of the module can be None
       						in which case this is defined to be part of the YANG language. The arg can also be none
       									 			  
       						We use a key a hash id that hashes the statement path.
       						Statement path is defined as (/statement-path)+
        						statement-path := (<name_of_module>, keyword, arg) 
        						The hash is necessary to identify the statement in YANG and has no meaning other wise";
      				   		type string;
      				  }
      				    
      				  leaf module-name {
      				        description "Module name of the extension that defines this keyword. Otherwise nothing.";
      				    	type string;
      				   }
      				    
      				   leaf keyword {
      					description "Keyword of the statement";
      				   	type string;
      				    }
      				
      				    leaf arg {
      					description "Optional argument";
      				  	type string;
      				    }
      				
      				    leaf parent {
      					 description "The id of the parent of this statement";
      					  type leafref {
      						path /ydk:ydk/ydk:modules/ydk:module/ydk:statements/ydk:statement/ydk:id;
      					   }
      				     }
      					
      				     leaf-list substatements {
      					    description "Substatements for this statement";
      					    type leafref {
      						path /ydk:ydk/ydk:modules/ydk:module/ydk:statements/ydk:statement/ydk:id;
      					     }
      				     }
      					
      				}
      			}
      		}
      }
      			
      container service-providers {
      	config false;
      	
      	list service-provider {
      		description "Service Providers . A service provider 
      		augments this list. Note it is the responsibility of the
      		service provider to provide an id 
      	
      	module providers {
      	  ...
      	  import ydk {
      	  	prefix ydk;
      	  }
      	  
      	  
      	  augment /ydk:service-providers/ydk:service-provider {
      	  	    ydk:service-provider-class-name NetconfServiceProvider;
      	  			
      	  		leaf address {
      	  			type inet:address;
      	  		}
      	  		
      	  		leaf port {
      	  			type uint16;
      	  		}
      	  		
      	  		leaf username {
      	  			type string;
      	  		}
      	  		
      	  		leaf password {
      	  			type string;
      	  		}
      	  		
      	  		leaf protocol {
      	  			type enumeration {
      	  				enum SSH;
      	  				
      	  				enum TCP;
      	  			}
      	  		}
      	  	
      	  	}
      	  
      	  }
      	
      	}
      	
      	";
      	
      		key name;
      		
      		leaf name {
      			description "A unique name identifying this service-provider within the system";
      			type string;
      		}
      		
      		leaf-list capabilities {
      			description "List of capabilities";
      			type string;
      		}
      	}
      }
      container bundles {	
      	config false;
      	
      	list bundle {
      		
      		description "A bundle definition. Each bundle consists of a list of modules/submodules that 
      		are used by ydk-gen to generate the API for this bundle. 
      		
      		It also lists other bundles that this bundle is dependent on. Note that list of modules/submodules 
      		collected from this bundle's modules and its dependent bundles must be self contained. i.e all cross
      		references must be resolvable from this set of modules.'";
      		
      	    key "name major-version-number minor-version-number";
      	    
      		leaf name {
      		  description "Name of the bundle. This is usually used as the root package.";
      		  
      		  type string;
      		}
      		
      		leaf description-url {
      			type string;
      		}
      		
      		leaf author {
      			type string;
      		}
      		
      		leaf author_email {
      			type string;
      		}
      		
      		leaf license {
      			type string;
      		}
      		
      		uses semantic-version;
      		
      		container ydk-version {
      			description "YDK version this bundle is dependent on.";
      			uses semantic-version;
      		}
      		
      		
      		container bundle-dependencies {
      		
      		   description "This would list all bundles that this bundle is dependent on.";
      		   
      		   list bundle-dependency {
      		       description "A bundle the parent bundle is dependent on.";
      		       
      		 	   key name;
      		 	   
      		 	   leaf name {
      		 	   	 description "Name of the bundle this bundle is dependent on .";
      		 	   	 
      		 	      type leafref {
      		 	          path "/ydk:ydk/ydk:bundles/ydk:bundle/ydk:name"; 
      		 	      }  
      		   	   }
      		   	   
      		   	   uses semantic-version;
      		   	 
      		   	   
      				leaf uri {
      					
      					type string;
      				
      					description "Either file://.. path to file or git://<path to .git repo>?commit-id=<commit-id>?path=<relative-path-to-file>
     					";
      				}   	   
      		 		
      		  }
      	    }
      		
      		container named-elements {
      			description "Named elements exported by this bundle";
      				
      			list named-element {
      				key fqn;
      				
      				leaf fqn {
      				
      					type string;
      					
      					description "The fully qualified name of this element.";
      				}
      				
      				leaf qn {
      					type string;
      					
      					description "The qualified name of this element.";
      				}
      				
      				leaf owner {
    					type leafref {
    						path "/ydk:ydk/ydk:bundles/ydk:bundle/ydk:named-elements/ydk:named-element/ydk:fqn";
    					}
    				
    				}
    			
    				leaf-list owned_elements {
    					type leafref {
    						path "/ydk:ydk/ydk:bundles/ydk:bundle/ydk:named-elements/ydk:named-element/ydk:fqn";
    					}
    				}
    		
      				
      				leaf module-name {
      					type leafref {
      						path "/ydk:ydk/ydk:modules/ydk:module/ydk:name" ;
      					}
      				}
      				
      				leaf module-revision {
      					type leafref {
      						path "/ydk:ydk/ydk:modules/ydk:module/ydk:revision";
      					}
      				}
      				
      				leaf statement-id {
      					type leafref {
      						path "/ydk:ydk/ydk:modules/ydk:module/ydk:statements/ydk:statement/ydk:id";
      					}
      				}
      				
      				leaf python-module-name {
      				    type string;
      				 }
      				
      				choice named-element {
      	
      				    case typed-element {
      				    	leaf lower-bound {
      				    		type int32;
      				    	}
      				    	
      				    	leaf upper-bound {
      				    		type int32;
      				    	}
      				    	
      				    	leaf many {
      				    		type empty;
      				    	}
      				    	
      				    	leaf ordered {
      				    		type empty;
      				    	}
      				    	
      				    	leaf required {
      				    		type empty;
      				    	}
      				    	
      				    	leaf unique {
      				    		type empty;
      				    	}
      				    	
      				    	
      				    	
      				    	choice typed-element {
      				    	
      				    		case structural-feature {
      				    			leaf feature-id {
      				    				type int32;
      				    			}
      				    			
      				    			leaf changeable {
      				    				type empty;
      				    			}
      				    			
      				    			leaf derived {
      				    				type empty;
      				    			}
      				    			
      				    			leaf transient {
      				    				type empty;
      				    			}
      				    			
      				    			choice structural-feature {
      				    				case attribute {
      				    					leaf attribute-type {
      				    						description "This has to point a data-type";
      				    						type leafref {
    												path "/ydk:ydk/ydk:bundles/ydk:bundle/ydk:named-elements/ydk:named-element/ydk:fqn";
    											}
      				    					}
      				    					
      				    					leaf id {
      				    						description "Is this attribute a key";
      				    						type empty;
      				    					}
      				    				}
      				    				
      				    				case reference {
      				    					leaf containment {
      				    						description "A reference is a containment if it
      				    								represents by-value content.";
      				    						type empty;
      				    					}
      				    					
      				    					leaf container {
      				    						description "A reference is a container if has an opposite that is 
      				    						a containment";
      				    						type empty;
      				    					}
      				    					
      				    					leaf opposite {
      				    						description "Represents the other end of a bidirectional relation";
      				    						type leafref {
    												path "/ydk:ydk/ydk:bundles/ydk:bundle/ydk:named-elements/ydk:named-element/ydk:fqn";
    											}
      				    					}
      				    				}
      				    			}
      				    		
      				    		}
      				    	} /* choice typed element */
      				    	
      				    } /* case typed element */
      				    
      				    
      					case package {
      						leaf-list subpackages {
      							type leafref {
    								path "/ydk:ydk/ydk:bundles/ydk:bundle/ydk:named-elements/ydk:named-element/ydk:fqn";
    							}
      						}
      						
      					}
      					
      					case enum-literal {
      						leaf literal {
      							type string;
      						}
      						
      						leaf value {
      							type int32;
      						}
      					}
      					
      					case classifier {
      					    
      					    leaf classifier-id {
      					    	type int32;
      					    }
      					    
      					    leaf package {
      					    	type leafref {
    								path "/ydk:ydk/ydk:bundles/ydk:bundle/ydk:named-elements/ydk:named-element/ydk:fqn";
    							}
      					    }
      					    
      						choice classifier {
      							case bits {
      								
      							}
      						
      							case class {
      								leaf-list all-attributes{
      					    			type leafref {
    										path "/ydk:ydk/ydk:bundles/ydk:bundle/ydk:named-elements/ydk:named-element/ydk:fqn";
    									}
      					    		}
      					    		
      					    		leaf-list all-containments{
      					    			type leafref {
    										path "/ydk:ydk/ydk:bundles/ydk:bundle/ydk:named-elements/ydk:named-element/ydk:fqn";
    									}
      					    		}
      					    		
      					    		leaf-list all-structural-features{
      					    			type leafref {
    										path "/ydk:ydk/ydk:bundles/ydk:bundle/ydk:named-elements/ydk:named-element/ydk:fqn";
    									}
      					    		}
      					    		
      					    		leaf-list all-super-types {
      					    			type leafref {
    										path "/ydk:ydk/ydk:bundles/ydk:bundle/ydk:named-elements/ydk:named-element/ydk:fqn";
    									}
      					    		}
      					    		
      					    		leaf-list attributes{
      					    			type leafref {
    										path "/ydk:ydk/ydk:bundles/ydk:bundle/ydk:named-elements/ydk:named-element/ydk:fqn";
    									}
      					    		}
      					    		
      					    		leaf-list references{
      					    			type leafref {
    										path "/ydk:ydk/ydk:bundles/ydk:bundle/ydk:named-elements/ydk:named-element/ydk:fqn";
    									}
      					    		}
      					    		
      					    		leaf-list structural-features{
      					    			type leafref {
    										path "/ydk:ydk/ydk:bundles/ydk:bundle/ydk:named-elements/ydk:named-element/ydk:fqn";
    									}
      					    		}
      					    		
      					    		leaf-list super-types {
      					    			type leafref {
    										path "/ydk:ydk/ydk:bundles/ydk:bundle/ydk:named-elements/ydk:named-element/ydk:fqn";
    									}
      					    		}
      					    		
      					    		leaf abstract {
      					    			type empty;
      					    		}
      					    		
      							}
      							
      							case data-type {
      								choice data-type {
      									case enum {
      										leaf literals {
      											type leafref {
    												path "/ydk:ydk/ydk:bundles/ydk:bundle/ydk:named-elements/ydk:named-element/ydk:fqn";
    											}
    										}
      									}
      								} 
      							}
      						}
      					}
      				}
      				
      			}
      			
      		}
      		
      	    container source-modules {
      	    	description "Collection of modules/submodules that is part of this bundle";
      	    	
      	    	list source-module{
      	    		description "Module or submodule that is part of this bundle";
      	    		key "name revision";
      	    		
      	    		leaf name{
      	    			type leafref {
      	    				path /ydk:ydk/ydk:modules/ydk:module/ydk:name;
      	    			}
      	    		}
      	    		
      	    		leaf revision {
      	    			type leafref {
      	    				path /ydk:ydk/ydk:modules/ydk:module/ydk:revision;
      	    			}
      	    		}
      	    		
      			}
      		}
      	    		
     		container api-generation-strategy {
     			presence "If this is instiated it usually means that a modification to the default
     			code generation strategy is being requested. The default strategy uses the expanded
     			api syntax. In the future other options can be added in here. Note the user is advised
     			to consider the implications of each code generation-strategy when they they design their bundle.";
     			
     			
     			leaf grouping-as-superclass {
     				
     				description "In this strategy groupings become classes in the module they are defined.
     				Any container/ list that uses the grouping becomes a subclass of the grouping-class.
     				This does reduce the number of classes generated. However it may break backward compatibility
     				since the grouping now becomes part of the api.";
     				type empty;
     			}
     			
     			leaf collapse-containers-with-single-list {
     				
     				description "Consider the following pattern in yang
     				  
     				  list bundle {
     				  	.....
     				  	container modules {
     				  			list module {
     				  			...
     				  
     				  In the expanded API generations strategy the source file will be encoded as follows
     				  
     				  class Bundle(object): --> corresponds to the list bundle
     				  	def __init__(self):
     				  		self.modules = Modules() 
     				  		
     				  	class Modules(object): --> corresponds to the container modules
     				    	def __init__(self):
     				       		self.module = YList() --> corresponds to the list Module
     				       
     				    	class Module(object): --> corresponds to the list module     
     				  	
    					If this leaf is set the generated api will skip the container modules and have the class
    					 Module as a child of the Bundle class like this
    					 
    					   class Bundle(object):  ---> corresponds to the list bundle
    					      def __init__(self):
    					      	 self.modules = YList() --> corresponds to the list Module
    					      
    					      class Module(obect):
    					           ... 				  	
     				  	
     				  	
     				  ";
     				type empty;
     			}
     		}
      
     }
     }
      
     }
      
      rpc generate {
        description "Generate APIs for the given set of bundles";
        input {
        	leaf-list bundles {
        	  description "Path to the bundle file. This will follow the uri syntax.
        	  Note although this is a list, the implementation may choose to support only one uri.";
        	  type string;
        	}
        	leaf output-directory {
        		description "The output directory where the generated files will be put
        		The structure of this directory will follow
        		 language bundle.
        		 
        		 For eg- 
        		 
        		  +-<output-dir>
        		    |
        		    +-python
        		      |
        		      + cisco-xr600
        		        |
        		          - __init__.py
        		            Cisco_IOS_XR_bgp_cfg.py..
        		      
        		";
        		type string;
        	
        	}
        	leaf-list languages {
        		description "The target language for the generated api";
        		type enumeration {
        			enum python {
        				value 1;
        			}
        		}
        	}
        	leaf reuse-modules-cache {
        		description "If set the generator will use the cache for modules find in the output directory.
        					This allows a previously downloaded file to be used.";
        		type empty;
        	}
        	leaf reuse-bundles-cache {
        		description "If set the generator will reuse the bundle cache for modules found in the output directory. 
        				This allows a previously downloaded file to be used.";
        		type empty;
        	}
        }
      }
      rpc create {
        description "The create rpc call to create an entity. This is the model for the CRUDService offered by ydk";
        input {
            anyxml entity {
               description "The xml like payload that represents the entity or entities to create"; 
            }
        }
      } 

      rpc read {
          description "Read the entity/entities";
         input {
            anyxml filter {
               description "Filter of what needs to be read. Modelled like the netconf get/get-config filter";
            }

            leaf only-config {
                description "If set filter on config elements only";
                type empty;
            } 
         }
         output {
             anyxml data {
               description "Data that is read";
             }
         }
      }
      rpc update {
        description "The update rpc call to update an entity. This is the model for the CRUDService offered by ydk";
        input {
            anyxml entity {
               description "The xml like payload that represents the entity or entities to update"; 
            }
        }
      } 
      rpc delete {
        description "The delete rpc call to delete an entity. This is the model for the CRUDService offered by ydk";
        input {

            anyxml entity {
               description "The xml like payload that represents the entity to delete."; 
            }
        }
      } 

    }
    )";
    }

    const char* IETF_NETCONF_MODULE_NAME = "ietf-netconf";
	const char* IETF_NETCONF_MODULE_REVISION = "2011-06-01";
	const char* IETF_NETCONF_MODULE = R"(module ietf-netconf {
	  // the namespace for NETCONF XML definitions is unchanged
	  // from RFC 4741, which this document replaces
	  namespace "urn:ietf:params:xml:ns:netconf:base:1.0";
	  prefix nc;
	  import ietf-inet-types {
		prefix inet;
	  }
	  organization
		"IETF NETCONF (Network Configuration) Working Group";
	  contact
		"WG Web:   <http://tools.ietf.org/wg/netconf/>
		 WG List:  <netconf@ietf.org>
		 WG Chair: Bert Wijnen
				   <bertietf@bwijnen.net>
		 WG Chair: Mehmet Ersue
				   <mehmet.ersue@nsn.com>
		 Editor:   Martin Bjorklund
				   <mbj@tail-f.com>
		 Editor:   Juergen Schoenwaelder
				   <j.schoenwaelder@jacobs-university.de>
		 Editor:   Andy Bierman
				   <andy.bierman@brocade.com>";
	  description
		"NETCONF Protocol Data Types and Protocol Operations.
		 Copyright (c) 2011 IETF Trust and the persons identified as
		 the document authors.  All rights reserved.
		 Redistribution and use in source and binary forms, with or
		 without modification, is permitted pursuant to, and subject
		 to the license terms contained in, the Simplified BSD License
		 set forth in Section 4.c of the IETF Trust's Legal Provisions
		 Relating to IETF Documents
		 (http://trustee.ietf.org/license-info).
		 This version of this YANG module is part of RFC 6241; see
		 the RFC itself for full legal notices.";
	  revision 2011-06-01 {
		description
		  "Initial revision";
		reference
		  "RFC 6241: Network Configuration Protocol";
	  }
	  extension get-filter-element-attributes {
		description
		  "If this extension is present within an 'anyxml'
		   statement named 'filter', which must be conceptually
		   defined within the RPC input section for the <get>
		   and <get-config> protocol operations, then the
		   following unqualified XML attribute is supported
		   within the <filter> element, within a <get> or
		   <get-config> protocol operation:
			 type : optional attribute with allowed
					value strings 'subtree' and 'xpath'.
					If missing, the default value is 'subtree'.
		   If the 'xpath' feature is supported, then the
		   following unqualified XML attribute is
		   also supported:
			 select: optional attribute containing a
					 string representing an XPath expression.
					 The 'type' attribute must be equal to 'xpath'
					 if this attribute is present.";
	  }
	  // NETCONF capabilities defined as features
	  feature writable-running {
		description
		  "NETCONF :writable-running capability;
		   If the server advertises the :writable-running
		   capability for a session, then this feature must
		   also be enabled for that session.  Otherwise,
		   this feature must not be enabled.";
		reference "RFC 6241, Section 8.2";
	  }
	  feature candidate {
		description
		  "NETCONF :candidate capability;
		   If the server advertises the :candidate
		   capability for a session, then this feature must
		   also be enabled for that session.  Otherwise,
		   this feature must not be enabled.";
		reference "RFC 6241, Section 8.3";
	  }
	  feature confirmed-commit {
		if-feature candidate;
		description
		  "NETCONF :confirmed-commit:1.1 capability;
		   If the server advertises the :confirmed-commit:1.1
		   capability for a session, then this feature must
		   also be enabled for that session.  Otherwise,
		   this feature must not be enabled.";
		reference "RFC 6241, Section 8.4";
	  }
	  feature rollback-on-error {
		description
		  "NETCONF :rollback-on-error capability;
		   If the server advertises the :rollback-on-error
		   capability for a session, then this feature must
		   also be enabled for that session.  Otherwise,
		   this feature must not be enabled.";
		reference "RFC 6241, Section 8.5";
	  }
	  feature validate {
		description
		  "NETCONF :validate:1.1 capability;
		   If the server advertises the :validate:1.1
		   capability for a session, then this feature must
		   also be enabled for that session.  Otherwise,
		   this feature must not be enabled.";
		reference "RFC 6241, Section 8.6";
	  }
	  feature startup {
		description
		  "NETCONF :startup capability;
		   If the server advertises the :startup
		   capability for a session, then this feature must
		   also be enabled for that session.  Otherwise,
		   this feature must not be enabled.";
		reference "RFC 6241, Section 8.7";
	  }
	  feature url {
		description
		  "NETCONF :url capability;
		   If the server advertises the :url
		   capability for a session, then this feature must
		   also be enabled for that session.  Otherwise,
		   this feature must not be enabled.";
		reference "RFC 6241, Section 8.8";
	  }
	  feature xpath {
		description
		  "NETCONF :xpath capability;
		   If the server advertises the :xpath
		   capability for a session, then this feature must
		   also be enabled for that session.  Otherwise,
		   this feature must not be enabled.";
		reference "RFC 6241, Section 8.9";
	  }
	  // NETCONF Simple Types
	  typedef session-id-type {
		type uint32 {
		  range "1..max";
		}
		description
		  "NETCONF Session Id";
	  }
	  typedef session-id-or-zero-type {
		type uint32;
		description
		  "NETCONF Session Id or Zero to indicate none";
	  }
	  typedef error-tag-type {
		type enumeration {
		   enum in-use {
			 description
			   "The request requires a resource that
				already is in use.";
		   }
		   enum invalid-value {
			 description
			   "The request specifies an unacceptable value for one
				or more parameters.";
		   }
		   enum too-big {
			 description
			   "The request or response (that would be generated) is
				too large for the implementation to handle.";
		   }
		   enum missing-attribute {
			 description
			   "An expected attribute is missing.";
		   }
		   enum bad-attribute {
			 description
			   "An attribute value is not correct; e.g., wrong type,
				out of range, pattern mismatch.";
		   }
		   enum unknown-attribute {
			 description
			   "An unexpected attribute is present.";
		   }
		   enum missing-element {
			 description
			   "An expected element is missing.";
		   }
		   enum bad-element {
			 description
			   "An element value is not correct; e.g., wrong type,
				out of range, pattern mismatch.";
		   }
		   enum unknown-element {
			 description
			   "An unexpected element is present.";
		   }
		   enum unknown-namespace {
			 description
			   "An unexpected namespace is present.";
		   }
		   enum access-denied {
			 description
			   "Access to the requested protocol operation or
				data model is denied because authorization failed.";
		   }
		   enum lock-denied {
			 description
			   "Access to the requested lock is denied because the
				lock is currently held by another entity.";
		   }
		   enum resource-denied {
			 description
			   "Request could not be completed because of
				insufficient resources.";
		   }
		   enum rollback-failed {
			 description
			   "Request to roll back some configuration change (via
				rollback-on-error or <discard-changes> operations)
				was not completed for some reason.";
		   }
		   enum data-exists {
			 description
			   "Request could not be completed because the relevant
				data model content already exists.  For example,
				a 'create' operation was attempted on data that
				already exists.";
		   }
		   enum data-missing {
			 description
			   "Request could not be completed because the relevant
				data model content does not exist.  For example,
				a 'delete' operation was attempted on
				data that does not exist.";
		   }
		   enum operation-not-supported {
			 description
			   "Request could not be completed because the requested
				operation is not supported by this implementation.";
		   }
		   enum operation-failed {
			 description
			   "Request could not be completed because the requested
				operation failed for some reason not covered by
				any other error condition.";
		   }
		   enum partial-operation {
			 description
			   "This error-tag is obsolete, and SHOULD NOT be sent
				by servers conforming to this document.";
		   }
		   enum malformed-message {
			 description
			   "A message could not be handled because it failed to
				be parsed correctly.  For example, the message is not
				well-formed XML or it uses an invalid character set.";
		   }
		 }
		 description "NETCONF Error Tag";
		 reference "RFC 6241, Appendix A";
	  }
	  typedef error-severity-type {
		type enumeration {
		  enum error {
			description "Error severity";
		  }
		  enum warning {
			description "Warning severity";
		  }
		}
		description "NETCONF Error Severity";
		reference "RFC 6241, Section 4.3";
	  }
	  typedef edit-operation-type {
		type enumeration {
		  enum merge {
			description
			  "The configuration data identified by the
			   element containing this attribute is merged
			   with the configuration at the corresponding
			   level in the configuration datastore identified
			   by the target parameter.";
		  }
		  enum replace {
			description
			  "The configuration data identified by the element
			   containing this attribute replaces any related
			   configuration in the configuration datastore
			   identified by the target parameter.  If no such
			   configuration data exists in the configuration
			   datastore, it is created.  Unlike a
			   <copy-config> operation, which replaces the
			   entire target configuration, only the configuration
			   actually present in the config parameter is affected.";
		  }
		  enum create {
			description
			  "The configuration data identified by the element
			   containing this attribute is added to the
			   configuration if and only if the configuration
			   data does not already exist in the configuration
			   datastore.  If the configuration data exists, an
			   <rpc-error> element is returned with an
			   <error-tag> value of 'data-exists'.";
		  }
		  enum delete {
			description
			  "The configuration data identified by the element
			   containing this attribute is deleted from the
			   configuration if and only if the configuration
			   data currently exists in the configuration
			   datastore.  If the configuration data does not
			   exist, an <rpc-error> element is returned with
			   an <error-tag> value of 'data-missing'.";
		  }
		  enum remove {
			description
			  "The configuration data identified by the element
			   containing this attribute is deleted from the
			   configuration if the configuration
			   data currently exists in the configuration
			   datastore.  If the configuration data does not
			   exist, the 'remove' operation is silently ignored
			   by the server.";
		  }
		}
		default "merge";
		description "NETCONF 'operation' attribute values";
		reference "RFC 6241, Section 7.2";
	  }
	  // NETCONF Standard Protocol Operations
	  rpc get-config {
		description
		  "Retrieve all or part of a specified configuration.";
		reference "RFC 6241, Section 7.1";
		input {
		  container source {
			description
			  "Particular configuration to retrieve.";
			choice config-source {
			  mandatory true;
			  description
				"The configuration to retrieve.";
			  leaf candidate {
				if-feature candidate;
				type empty;
				description
				  "The candidate configuration is the config source.";
			  }
			  leaf running {
				type empty;
				description
				  "The running configuration is the config source.";
			  }
			  leaf startup {
				if-feature startup;
				type empty;
				description
				  "The startup configuration is the config source.
				   This is optional-to-implement on the server because
				   not all servers will support filtering for this
				   datastore.";
			  }
			}
		  }
		  anyxml filter {
			description
			  "Subtree or XPath filter to use.";
			nc:get-filter-element-attributes;
		  }
		}
		output {
		  anyxml data {
			description
			  "Copy of the source datastore subset that matched
			   the filter criteria (if any).  An empty data container
			   indicates that the request did not produce any results.";
		  }
		}
	  }
	  rpc edit-config {
		description
		  "The <edit-config> operation loads all or part of a specified
		   configuration to the specified target configuration.";
		reference "RFC 6241, Section 7.2";
		input {
		  container target {
			description
			  "Particular configuration to edit.";
			choice config-target {
			  mandatory true;
			  description
				"The configuration target.";
			  leaf candidate {
				if-feature candidate;
				type empty;
				description
				  "The candidate configuration is the config target.";
			  }
			  leaf running {
				if-feature writable-running;
				type empty;
				description
				  "The running configuration is the config source.";
			  }
			}
		  }
		  leaf default-operation {
			type enumeration {
			  enum merge {
				description
				  "The default operation is merge.";
			  }
			  enum replace {
				description
				  "The default operation is replace.";
			  }
			  enum none {
				description
				  "There is no default operation.";
			  }
			}
			default "merge";
			description
			  "The default operation to use.";
		  }
		  leaf test-option {
			if-feature validate;
			type enumeration {
			  enum test-then-set {
				description
				  "The server will test and then set if no errors.";
			  }
			  enum set {
				description
				  "The server will set without a test first.";
			  }
			  enum test-only {
				description
				  "The server will only test and not set, even
				   if there are no errors.";
			  }
			}
			default "test-then-set";
			description
			  "The test option to use.";
		  }
		  leaf error-option {
			type enumeration {
			  enum stop-on-error {
				description
				  "The server will stop on errors.";
			  }
			  enum continue-on-error {
				description
				  "The server may continue on errors.";
			  }
			  enum rollback-on-error {
				description
				  "The server will roll back on errors.
				   This value can only be used if the 'rollback-on-error'
				   feature is supported.";
			  }
			}
			default "stop-on-error";
			description
			  "The error option to use.";
		  }
		  choice edit-content {
			mandatory true;
			description
			  "The content for the edit operation.";
			anyxml config {
			  description
				"Inline Config content.";
			}
			leaf url {
			  if-feature url;
			  type inet:uri;
			  description
				"URL-based config content.";
			}
		  }
		}
	  }
	  rpc copy-config {
		description
		  "Create or replace an entire configuration datastore with the
		   contents of another complete configuration datastore.";
		reference "RFC 6241, Section 7.3";
		input {
		  container target {
			description
			  "Particular configuration to copy to.";
			choice config-target {
			  mandatory true;
			  description
				"The configuration target of the copy operation.";
			  leaf candidate {
				if-feature candidate;
				type empty;
				description
				  "The candidate configuration is the config target.";
			  }
			  leaf running {
				if-feature writable-running;
				type empty;
				description
				  "The running configuration is the config target.
				   This is optional-to-implement on the server.";
			  }
			  leaf startup {
				if-feature startup;
				type empty;
				description
				  "The startup configuration is the config target.";
			  }
			  leaf url {
				if-feature url;
				type inet:uri;
				description
				  "The URL-based configuration is the config target.";
			  }
			}
		  }
		  container source {
			description
			  "Particular configuration to copy from.";
			choice config-source {
			  mandatory true;
			  description
				"The configuration source for the copy operation.";
			  leaf candidate {
				if-feature candidate;
				type empty;
				description
				  "The candidate configuration is the config source.";
			  }
			  leaf running {
				type empty;
				description
				  "The running configuration is the config source.";
			  }
			  leaf startup {
				if-feature startup;
				type empty;
				description
				  "The startup configuration is the config source.";
			  }
			  leaf url {
				if-feature url;
				type inet:uri;
				description
				  "The URL-based configuration is the config source.";
			  }
			  anyxml config {
				description
				  "Inline Config content: <config> element.  Represents
				   an entire configuration datastore, not
				   a subset of the running datastore.";
			  }
			}
		  }
		}
	  }
	  rpc delete-config {
		description
		  "Delete a configuration datastore.";
		reference "RFC 6241, Section 7.4";
		input {
		  container target {
			description
			  "Particular configuration to delete.";
			choice config-target {
			  mandatory true;
			  description
				"The configuration target to delete.";
			  leaf startup {
				if-feature startup;
				type empty;
				description
				  "The startup configuration is the config target.";
			  }
			  leaf url {
				if-feature url;
				type inet:uri;
				description
				  "The URL-based configuration is the config target.";
			  }
			}
		  }
		}
	  }
	  rpc lock {
		description
		  "The lock operation allows the client to lock the configuration
		   system of a device.";
		reference "RFC 6241, Section 7.5";
		input {
		  container target {
			description
			  "Particular configuration to lock.";
			choice config-target {
			  mandatory true;
			  description
				"The configuration target to lock.";
			  leaf candidate {
				if-feature candidate;
				type empty;
				description
				  "The candidate configuration is the config target.";
			  }
			  leaf running {
				type empty;
				description
				  "The running configuration is the config target.";
			  }
			  leaf startup {
				if-feature startup;
				type empty;
				description
				  "The startup configuration is the config target.";
			  }
			}
		  }
		}
	  }
	  rpc unlock {
		description
		  "The unlock operation is used to release a configuration lock,
		   previously obtained with the 'lock' operation.";
		reference "RFC 6241, Section 7.6";
		input {
		  container target {
			description
			  "Particular configuration to unlock.";
			choice config-target {
			  mandatory true;
			  description
				"The configuration target to unlock.";
			  leaf candidate {
				if-feature candidate;
				type empty;
				description
				  "The candidate configuration is the config target.";
			  }
			  leaf running {
				type empty;
				description
				  "The running configuration is the config target.";
			  }
			  leaf startup {
				if-feature startup;
				type empty;
				description
				  "The startup configuration is the config target.";
			  }
			}
		  }
		}
	  }
	  rpc get {
		description
		  "Retrieve running configuration and device state information.";
		reference "RFC 6241, Section 7.7";
		input {
		  anyxml filter {
			description
			  "This parameter specifies the portion of the system
			   configuration and state data to retrieve.";
			nc:get-filter-element-attributes;
		  }
		}
		output {
		  anyxml data {
			description
			  "Copy of the running datastore subset and/or state
			   data that matched the filter criteria (if any).
			   An empty data container indicates that the request did not
			   produce any results.";
		  }
		}
	  }
	  rpc close-session {
		description
		  "Request graceful termination of a NETCONF session.";
		reference "RFC 6241, Section 7.8";
	  }
	  rpc kill-session {
		description
		  "Force the termination of a NETCONF session.";
		reference "RFC 6241, Section 7.9";
		input {
		  leaf session-id {
			type session-id-type;
			mandatory true;
			description
			  "Particular session to kill.";
		  }
		}
	  }
	  rpc commit {
		if-feature candidate;
		description
		  "Commit the candidate configuration as the device's new
		   current configuration.";
		reference "RFC 6241, Section 8.3.4.1";
		input {
		  leaf confirmed {
			if-feature confirmed-commit;
			type empty;
			description
			  "Requests a confirmed commit.";
			reference "RFC 6241, Section 8.3.4.1";
		  }
		  leaf confirm-timeout {
			if-feature confirmed-commit;
			type uint32 {
			  range "1..max";
			}
			units "seconds";
			default "600";   // 10 minutes
			description
			  "The timeout interval for a confirmed commit.";
			reference "RFC 6241, Section 8.3.4.1";
		  }
		  leaf persist {
			if-feature confirmed-commit;
			type string;
			description
			  "This parameter is used to make a confirmed commit
			   persistent.  A persistent confirmed commit is not aborted
			   if the NETCONF session terminates.  The only way to abort
			   a persistent confirmed commit is to let the timer expire,
			   or to use the <cancel-commit> operation.
			   The value of this parameter is a token that must be given
			   in the 'persist-id' parameter of <commit> or
			   <cancel-commit> operations in order to confirm or cancel
			   the persistent confirmed commit.
			   The token should be a random string.";
			reference "RFC 6241, Section 8.3.4.1";
		  }
		  leaf persist-id {
			if-feature confirmed-commit;
			type string;
			description
			  "This parameter is given in order to commit a persistent
			   confirmed commit.  The value must be equal to the value
			   given in the 'persist' parameter to the <commit> operation.
			   If it does not match, the operation fails with an
			  'invalid-value' error.";
			reference "RFC 6241, Section 8.3.4.1";
		  }
		}
	  }
	  rpc discard-changes {
		if-feature candidate;
		description
		  "Revert the candidate configuration to the current
		   running configuration.";
		reference "RFC 6241, Section 8.3.4.2";
	  }
	  rpc cancel-commit {
		if-feature confirmed-commit;
		description
		  "This operation is used to cancel an ongoing confirmed commit.
		   If the confirmed commit is persistent, the parameter
		   'persist-id' must be given, and it must match the value of the
		   'persist' parameter.";
		reference "RFC 6241, Section 8.4.4.1";
		input {
		  leaf persist-id {
			type string;
			description
			  "This parameter is given in order to cancel a persistent
			   confirmed commit.  The value must be equal to the value
			   given in the 'persist' parameter to the <commit> operation.
			   If it does not match, the operation fails with an
			  'invalid-value' error.";
		  }
		}
	  }
	  rpc validate {
		if-feature validate;
		description
		  "Validates the contents of the specified configuration.";
		reference "RFC 6241, Section 8.6.4.1";
		input {
		  container source {
			description
			  "Particular configuration to validate.";
			choice config-source {
			  mandatory true;
			  description
				"The configuration source to validate.";
			  leaf candidate {
				if-feature candidate;
				type empty;
				description
				  "The candidate configuration is the config source.";
			  }
			  leaf running {
				type empty;
				description
				  "The running configuration is the config source.";
			  }
			  leaf startup {
				if-feature startup;
				type empty;
				description
				  "The startup configuration is the config source.";
			  }
			  leaf url {
				if-feature url;
				type inet:uri;
				description
				  "The URL-based configuration is the config source.";
			  }
			  anyxml config {
				description
				  "Inline Config content: <config> element.  Represents
				   an entire configuration datastore, not
				   a subset of the running datastore.";
			  }
			}
		  }
		}
	  }
	})";
}
