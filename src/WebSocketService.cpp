#include "WebSocketService.h"

WebSocketService::WebSocketService(
	const String &url,
	void (*log)(const char *) /*= defaultLog*/
): url(url), connected(false), log(log) {
	this->setupEventDriver();
}

WebSocketService::~WebSocketService() {
	this->close();
	this->connected = false;
};

bool WebSocketService::poll() {
	connected = this->available();
	if (connected) {
		websockets::WebsocketsClient::poll();
	} else {
		this->handleConnection();
	}
	return connected;
}

void WebSocketService::handleConnection() {
	log("Connecting to "); log(url.c_str()); log("...\n");
	connected = this->connect(url);
	if (connected) {
		log("Connected!");
	} else {
		log("Not Connected!");
	}
}

static bool validate(const String &reducer) {
	return reducer.length() > 0;
}

template <class Message = WebSocketService::Message>
void WebSocketService::setupEventDriver() {
	this->onMessage([this](websockets::WebsocketsMessage msg){
		try {
			const String eventKey
				= Message(msg).type();
			const String payload
				= Message(msg).payload();
			const String reducer
				= Message(msg).handlerHint();
			const String senderIdentity
				= Message(msg).senderIdentity();
			log("Event: "); log(eventKey.c_str()); log(" Message: ");
			log(payload.c_str()); log("\n");
			log("Full msg: "); log(msg.c_str()); log("\n");
 			useHandleEvent(
				eventKey.c_str(),
				payload,
				validate(reducer) ? reducer.c_str() : nullptr,
				validate(senderIdentity) ? senderIdentity.c_str() : nullptr
			);
		} catch (const std::exception& e) {
			log("Exception caught: "); log(e.what()); log("\n");
			connected = false;
		}
	});
}

static void execute(
	WebSocketService::EventHandler &handler,
	const String &payload,
	const char *identity
) {
	if (identity != nullptr) {
		Serial.println("Executing with identity");
		Serial.println(payload);
		Serial.println(identity);
		if (handler.identify(identity)) {
			handler.execute(payload);
		}
	} else {
		handler.execute(payload);
	}
}

void WebSocketService::useHandleEvent(
	const char *eventKey,
	const String &payload,
	const char *channel, /*= nullptr*/
	const char *identity /*= nullptr*/
) {
	EventHandlerIterator it 
		= eventHandlerMap
			.find(std::string(eventKey));
	if (it != eventHandlerMap.end()) {
		if /*not defaulted*/ (channel != nullptr) {
			/*validate*/if (!it->second.filter(channel)) return;
		} /*then*/ try {
			execute(it->second, payload, identity);
		} /*but*/ catch (const std::exception &e) {
			log("Exception: ");  log(e.what()); log("\n");
			connected = false;
		}
	} else {
		log("Unknown event: "); log(eventKey); log("\n");
	}
}

void WebSocketService::defaultLog(const char *message) {
	(void)message;
}