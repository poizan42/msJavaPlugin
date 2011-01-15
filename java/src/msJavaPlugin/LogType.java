package msJavaPlugin;

public enum LogType {
	LOG_EMERG,    /** system is unusable */
	LOG_ALERT,    /** action must be taken immediately*/ 
	LOG_CRITICAL, /** critical conditions */
	LOG_ERROR,    /** error conditions */
	LOG_WARNING,  /** warning conditions */
	LOG_NOTICE,   /** normal, but significant, condition */
	LOG_INFO,     /** informational message */
	LOG_DEBUG     /** debug-level message */
}
