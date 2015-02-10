Ext.define("OMV.module.admin.service.flexget.Extra", {
    extend: "OMV.workspace.form.Panel",
    
    // This path tells which RPC module and methods this panel will call to get 
    // and fetch its form values.
    rpcService: "Flexget",
    rpcGetMethod: "getSettings",
    rpcSetMethod: "setSettings",

    getFormItems : function() {
        var me = this;
        return [{
            xtype    : "fieldset",
            title    : _("Available Tasks"),
            defaults : {
                labelSeparator : ""
            },
            items : [{
                xtype         : "combo",
                name          : "tasks",
                allowBlank    : false,
                editable      : false,
                triggerAction : "all",
                displayField  : "name",
                valueField    : "name",
                store         : Ext.create("OMV.data.Store", {
                    autoLoad : true,
                    model    : OMV.data.Model.createImplicit({
                        idProperty : "name",
                        fields     : [
                            { name : "name", type : "string" }
                        ]
                    }),
                    proxy : {
                        type    : "rpc",
                        rpcData : {
                            service : "flexget",
                            method  : "doGetTaskList"
                        }
                    }
                }),
                listeners     : {
                    scope  : me,
                    change : function(combo, value) {
                        me.packageName = value;
                    }
                }                
            },{
                xtype   : "button",
                name    : "install",
                text    : _("Run Selected Task"),
                scope   : this,
                handler : Ext.Function.bind(me.onInstallButton, me, [ me ]),
                margin  : "0 0 7 0"
            }]
        }];
    },
 });

OMV.WorkspaceManager.registerPanel({
    id: "extra", 
    path: "/service/flexget", 
    text: _("Extra"), 
    position: 20,
    className: "OMV.module.admin.service.flexget.Extra" 
});
