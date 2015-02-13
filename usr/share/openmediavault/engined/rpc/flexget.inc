<?php
require_once("openmediavault/config.inc");
require_once("openmediavault/error.inc");
require_once("openmediavault/notify.inc");
require_once("openmediavault/object.inc");
require_once("openmediavault/rpcservice.inc");
require_once("openmediavault/util.inc");
class OMVRpcServiceFlexget extends OMVRpcServiceAbstract
{
	/**
	 * Get the main event message path of the service. This is a helper 
	 * function to avoid "magic numbers".
	 *
	 * @return string
	 */
	private function getEventMessagePath()
	{
		return "org.openmediavault.services.flexget";
	}
	
	/**
	 * Get the base XPath of the service. This is a helper function to avoid 
	 * "magic numbers".
	 *
	 * @return string
	 */
	private function getXpath()
	{
		return "/config/services/flexget";
	}
	
	/**
	 * Get the name of the RPC service.
	 *
	 * @return string
	 */
	public function getName()
	{
		return "Flexget";
	}
	/**
	 * Initialize the RPC service. The RPC methods are registered in this
	 * function with $this->registerMethod.
	 *
	 * @return void
	 */
	public function initialize()
	{
		$this->registerMethod("getSettings");
		$this->registerMethod("setSettings");
        $this->registerMethod("doCheckSyntax");
        $this->registerMethod("doRunOnce");
        $this->registerMethod("doRunUpgrade");
        $this->registerMethod("doGetVersion");
        $this->registerMethod("doGetSeriesList");
        $this->registerMethod("doGetHistory");
	}

	public function getSettings($params, $context)
	{
		// $xmlConfig is needed when reading and writing from the configuration.
		global $xmlConfig;
		
		// Validate the RPC caller context.
		//
		// validateMethodContext takes the currentcontext as the first
		// parameter. The second paramter is the valid context and that can be
		// OMV_ROLE_ADMINISTRATOR, OMV_ROLE_USER or OMV_ROLE_EVERYONE.
		// This is used to make sure that the right user accesses the method.
		$this->validateMethodContext($context, array(
			"role" => OMV_ROLE_ADMINISTRATOR
		));
		
		// Get the configuration object.
		$object = $xmlConfig->get($this->getXpath());
		
		// If no data was found, throw an exception and provide the XPath that
		// failed.
		if (is_null($object)) {
			throw new OMVException(
				OMVErrorMsg::E_CONFIG_GET_OBJECT_FAILED,
				$this->getXpath()
			);
		}
		
		// Modify result data.
		// boolval and intval converts strings and numbers to their boolean
		// and integer value.
		$object["enable"] = boolval($object["enable"]);
		
		return $object;
	}

	public function setSettings($params, $context)
	{
		global $xmlConfig;
		
		$this->validateMethodContext($context, array(
			"role" => OMV_ROLE_ADMINISTRATOR
		));
		// Validate the parameters of the RPC service method.
		//
		// OpenMediavault uses JSON Schema to validate parameters. A more
		// detailed specification is provided here http://json-schema.org/
		$this->validateMethodParams(
			$params,
			'{
				"type": "object",
				"properties": {
					"enable": {
						"type": "boolean"
					},
					"config":{ 
						"type": "string"
					}
				}
			}'
		);
		// Update the configuration object.
		$object = array(
			"enable" => array_boolval($params, "enable"),
			"config" => $params["config"],
		);
		
		// Update the configuration file. If it fails it throws an exception.
		if (false === $xmlConfig->replace($this->getXpath(), $object)) {
			throw new OMVException(
				OMVErrorMsg::E_CONFIG_SET_OBJECT_FAILED,
				$this->getXpath()
			);
		}
		
		// Notify configuration changes.
		//
		// This will notify event listeners such as the service module
		// to perform certain tasks. The most common one is to mark the
		// service as dirty.
		$dispatcher = &OMVNotifyDispatcher::getInstance();
		$dispatcher->notify(
			OMV_NOTIFY_MODIFY,
			$this->getEventMessagePath(),
			$object
		);
		return $object;
	}

    public function doCheckSyntax($params, $context)
    {
        global $xmlConfig;
        
        $this->validateMethodContext(
            $context,
            array( "role" => OMV_ROLE_ADMINISTRATOR )
        );

        // Validate the parameters of the RPC service method.
        $this->validateMethodParams($params, '{
            "type"       : "object",
            "properties" : {
                "config"  : { "type" : "string" }
            }
        }');

        // Write temporary config to check
        $tmpFile = "/tmp/config.yml";

        if (file_exists($tmpFile))
            unlink($tmpFile);

        file_put_contents($tmpFile, $params['config']);

        // Create a background process.
        $bgStatusFilename = $this->createBgProcStatus();
        $pid = $this->fork();
        if ($pid > 0)   // Parent process.
        {
            $this->initializeBgProcStatus($bgStatusFilename, $pid);
            return $bgStatusFilename;
        }

        // Child process.
        try {
            $bgOutputFilename = $this->createBgProcOutput();
            $this->updateBgProcStatus($bgStatusFilename, "outputfilename", $bgOutputFilename);

            $cmd = sprintf("flexget -c %s check", $tmpFile);
            $this->exec($cmd, $output, $bgOutputFilename);
            $this->finalizeBgProcStatus($bgStatusFilename, $output);

        } catch(Exception $e) {
            $this->finalizeBgProcStatus($bgStatusFilename, "", $e);
            exit(1);
        }
    }

    public function doRunOnce($params, $context)
    {
        global $xmlConfig;
        
        $this->validateMethodContext(
            $context,
            array( "role" => OMV_ROLE_ADMINISTRATOR )
        );

        // Write temporary config to run once
        $configFile = "/home/flexget/.flexget/config.yml";

        if (file_exists($tmpFile))
            unlink($tmpFile);

        file_put_contents($tmpFile, $params['config']);

        // Create a background process.
        $bgStatusFilename = $this->createBgProcStatus();
        $pid = $this->fork();
        if ($pid > 0)   // Parent process.
        {
            $this->initializeBgProcStatus($bgStatusFilename, $pid);
            return $bgStatusFilename;
        }

        // Child process.
        try {
            $bgOutputFilename = $this->createBgProcOutput();
            $this->updateBgProcStatus($bgStatusFilename, "outputfilename", $bgOutputFilename);

            $cmd = sprintf("flexget -c %s execute", $configFile);
            $this->exec($cmd, $output, $bgOutputFilename);
            $this->finalizeBgProcStatus($bgStatusFilename, $output);

        } catch(Exception $e) {
            $this->finalizeBgProcStatus($bgStatusFilename, "", $e);
            exit(1);
        }
    }
    
    public function doGetSeriesList($params, $context)
    {
        global $xmlConfig;
        
        $this->validateMethodContext(
            $context,
            array( "role" => OMV_ROLE_ADMINISTRATOR )
        );

        // Create a background process.
        $bgStatusFilename = $this->createBgProcStatus();
        $pid = $this->fork();
        if ($pid > 0)   // Parent process.
        {
            $this->initializeBgProcStatus($bgStatusFilename, $pid);
            return $bgStatusFilename;
        }

        // Child process.
        try {
            $bgOutputFilename = $this->createBgProcOutput();
            $this->updateBgProcStatus($bgStatusFilename, "outputfilename", $bgOutputFilename);

            $cmd = "flexget -c /home/flexget/.flexget/config.yml series list";
            $this->exec($cmd, $output, $bgOutputFilename);
            $this->finalizeBgProcStatus($bgStatusFilename, $output);

        } catch(Exception $e) {
            $this->finalizeBgProcStatus($bgStatusFilename, "", $e);
            exit(1);
        }
    }    
    
    public function doGetHistory($params, $context)
    {
        global $xmlConfig;
        
        $this->validateMethodContext(
            $context,
            array( "role" => OMV_ROLE_ADMINISTRATOR )
        );

        // Create a background process.
        $bgStatusFilename = $this->createBgProcStatus();
        $pid = $this->fork();
        if ($pid > 0)   // Parent process.
        {
            $this->initializeBgProcStatus($bgStatusFilename, $pid);
            return $bgStatusFilename;
        }

        // Child process.
        try {
            $bgOutputFilename = $this->createBgProcOutput();
            $this->updateBgProcStatus($bgStatusFilename, "outputfilename", $bgOutputFilename);

            $cmd = "flexget -c /home/flexget/.flexget/config.yml history";
            $this->exec($cmd, $output, $bgOutputFilename);
            $this->finalizeBgProcStatus($bgStatusFilename, $output);

        } catch(Exception $e) {
            $this->finalizeBgProcStatus($bgStatusFilename, "", $e);
            exit(1);
        }
    }    


    public function doRunUpgrade($params, $context)
    {
        global $xmlConfig;
        
        $this->validateMethodContext(
            $context,
            array( "role" => OMV_ROLE_ADMINISTRATOR )
        );


        // Create a background process.
        $bgStatusFilename = $this->createBgProcStatus();
        $pid = $this->fork();
        if ($pid > 0)   // Parent process.
        {
            $this->initializeBgProcStatus($bgStatusFilename, $pid);
            return $bgStatusFilename;
        }

        // Child process.
        try {
            $bgOutputFilename = $this->createBgProcOutput();
            $this->updateBgProcStatus($bgStatusFilename, "outputfilename", $bgOutputFilename);

            $cmd = "pip install flexget --upgrade";
            $this->exec($cmd, $output, $bgOutputFilename);
            $this->finalizeBgProcStatus($bgStatusFilename, $output);

        } catch(Exception $e) {
            $this->finalizeBgProcStatus($bgStatusFilename, "", $e);
            exit(1);
        }
    }

    public function doGetVersion($params, $context)
    {
        global $xmlConfig;
        
        $this->validateMethodContext(
            $context,
            array( "role" => OMV_ROLE_ADMINISTRATOR )
        );

        // Create a background process.
        $bgStatusFilename = $this->createBgProcStatus();
        $pid = $this->fork();
        if ($pid > 0)   // Parent process.
        {
            $this->initializeBgProcStatus($bgStatusFilename, $pid);
            return $bgStatusFilename;
        }

        // Child process.
        try {
            $bgOutputFilename = $this->createBgProcOutput();
            $this->updateBgProcStatus($bgStatusFilename, "outputfilename", $bgOutputFilename);

            $cmd = "flexget -V";
            $this->exec($cmd, $output, $bgOutputFilename);
            $this->finalizeBgProcStatus($bgStatusFilename, $output);

        } catch(Exception $e) {
            $this->finalizeBgProcStatus($bgStatusFilename, "", $e);
            exit(1);
        }
    }    
 
}
// Register the RPC service.
$rpcServiceMgr = &OMVRpcServiceMgr::getInstance();
$rpcServiceMgr->registerService(new OMVRpcServiceFlexget());
