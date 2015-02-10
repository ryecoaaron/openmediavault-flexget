// require("js/omv/workspace/grid/Panel.js")

Ext.define("OMV.module.admin.service.flexget.Settings", {
    extend: "OMV.workspace.form.Panel",
    
    // This path tells which RPC module and methods this panel will call to get 
    // and fetch its form values.
    rpcService: "Flexget",
    rpcGetMethod: "getSettings",
    rpcSetMethod: "setSettings",

    getButtonItems : function() {
        var me = this;
        var items = me.callParent(arguments);
        items.push({
            id      : me.getId() + "-check",
            xtype   : "button",
            text    : _("Syntax"),
            icon    : "images/spell-check.png",
            iconCls : Ext.baseCSSPrefix + "btn-icon-16x16",
            scope   : me,
            handler : Ext.Function.bind(me.onCheckButton, me, [ me ])
        },{
            id      : me.getId() + "-runonce",
            xtype   : "button",
            text    : _("Run Once"),
            icon    : "images/play.png",
            iconCls : Ext.baseCSSPrefix + "btn-icon-16x16",
            scope   : me,
            handler : Ext.Function.bind(me.onRunOnceButton, me, [ me ])
        },{
			id      : me.getId() + "-runupgrade",
            xtype   : "button",
            text    : _("Upgrade"),
            icon    : "images/add.png",
            iconCls : Ext.baseCSSPrefix + "btn-icon-16x16",
            scope   : me,
            handler : Ext.Function.bind(me.onRunUpgradeButton, me, [ me ])
        },{
            id      : me.getId() + "-getinfo",
            xtype   : "button",
            text    : _("Info"),
            icon    : "images/info.png",
            iconCls : Ext.baseCSSPrefix + "btn-icon-16x16",
            scope   : me,
            handler : Ext.Function.bind(me.onGetVersionButton, me, [ me ])
		},{
			id: me.getId() + "-runsingletask",
			xtype: "triggerfield",
			flex: 1,
			emptyText: 'Run Single Task',
			trigger1Cls: Ext.baseCSSPrefix + "form-clear-trigger",
			trigger2Cls: Ext.baseCSSPrefix + "grid-page-nextr",
			onTrigger1Click: function() {
				// Reset the filter settings.
				this.reset();
				this.onTrigger2Click();
			},
			onTrigger2Click: function() {
				// Get the entered text that should be searched for.
				var pattern = this.getValue();
				var store = me.getStore();
				// Reset the filter setting.
				store.clearFilter(!Ext.isEmpty(pattern));
				// Prepare the new filter setting.
			},
        });
        return items;
    },

    // getFormItems is a method which is automatically called in the 
    // instantiation of the panel. This method returns all fields for 
    // the panel.
    getFormItems: function() {
        return [{
            // xtype defines the type of this entry. Some different types
            // is: fieldset, checkbox, textfield and numberfield. 
            xtype: "fieldset",
            title: _("General"),
            fieldDefaults: {
                labelSeparator: ""
            },
            // The items array contains items inside the fieldset xtype.
            items: [{
                xtype: "checkbox",
                // The name option is sent together with is value to RPC
                // and is also used when fetching from the RPC.
                name: "enable",
                fieldLabel: _("Enable"),
                // checked sets the default value of a checkbox.
                checked: false
            },
            {
                xtype: "textarea",
                name: "config",
                fieldLabel: _("Config.yml"),
                height: 1000,
                allowBlank: true
            }]
        }];
    },

    onCheckButton: function() {
        var me = this;
        var config = me.findField("config").getValue();
        var wnd = Ext.create("OMV.window.Execute", {
            title           : _("Checking config ..."),
            rpcService      : "Flexget",
            rpcMethod       : "doCheckSyntax",
            rpcParams      : {
                "config" : config
            },
            rpcIgnoreErrors : true,
            hideStartButton : true,
            hideStopButton  : true,
            listeners       : {
                scope     : me,
                finish    : function(wnd, response) {
                    wnd.appendValue(_("Done..."));
                    wnd.setButtonDisabled("close", false);
                },
                exception : function(wnd, error) {
                    OMV.MessageBox.error(null, error);
                    wnd.setButtonDisabled("close", false);
                }
            }
        });
        wnd.setButtonDisabled("close", true);
        wnd.show();
        wnd.start();
    },
    
    onRunOnceButton: function() {
        var me = this;
        var config = me.findField("config").getValue();
        var wnd = Ext.create("OMV.window.Execute", {
            title           : _("Sending execution task to the daemon"),
            rpcService      : "Flexget",
            rpcMethod       : "doRunOnce",
            rpcIgnoreErrors : true,
            hideStartButton : true,
            hideStopButton  : true,
            listeners       : {
                scope     : me,
                finish    : function(wnd, response) {
                    wnd.appendValue(_("Done..."));
                    wnd.setButtonDisabled("close", false);
                },
                exception : function(wnd, error) {
                    OMV.MessageBox.error(null, error);
                    wnd.setButtonDisabled("close", false);
                }
            }
        });
        wnd.setButtonDisabled("close", true);
        wnd.show();
        wnd.start();
    },
    
    onRunUpgradeButton: function() {
        var me = this;
        var config = me.findField("config").getValue();
        var wnd = Ext.create("OMV.window.Execute", {
            title           : _("Upgrading Flexget..."),
            rpcService      : "Flexget",
            rpcMethod       : "doRunUpgrade",
            rpcIgnoreErrors : true,
            hideStartButton : true,
            hideStopButton  : true,
            listeners       : {
                scope     : me,
                finish    : function(wnd, response) {
                    wnd.appendValue(_("Done..."));
                    wnd.setButtonDisabled("close", false);
                },
                exception : function(wnd, error) {
                    OMV.MessageBox.error(null, error);
                    wnd.setButtonDisabled("close", false);
                }
            }
        });
        wnd.setButtonDisabled("close", true);
        wnd.show();
        wnd.start();
    },
    
    onGetVersionButton: function() {
        var me = this;
        var config = me.findField("config").getValue();
        var wnd = Ext.create("OMV.window.Execute", {
            title           : _("Flexget Version Info"),
            rpcService      : "Flexget",
            rpcMethod       : "doGetVersion",
            rpcIgnoreErrors : true,
            hideStartButton : true,
            hideStopButton  : true,
            listeners       : {
                scope     : me,
                finish    : function(wnd, response) {
                    wnd.appendValue(_("Done..."));
                    wnd.setButtonDisabled("close", false);
                },
                exception : function(wnd, error) {
                    OMV.MessageBox.error(null, error);
                    wnd.setButtonDisabled("close", false);
                }
            }
        });
        wnd.setButtonDisabled("close", true);
        wnd.show();
        wnd.start();
    }
});
// Register a panel into the GUI.
//
// path: 
//     We want to add the panel in our flexget node. 
//     The node was configured with the path /service and the id flexget.
//     The path is therefore /service/flexget.
//
// className: 
//     The panel which should be registered and added (refers to 
//     the class name).
OMV.WorkspaceManager.registerPanel({
    id: "settings", 
    path: "/service/flexget", 
    text: _("Settings"), 
    position: 10,
    className: "OMV.module.admin.service.flexget.Settings" 
});
