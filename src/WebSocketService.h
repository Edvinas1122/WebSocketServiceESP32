#ifndef WEB_SOCKET_SERVICE_H
# define WEB_SOCKET_SERVICE_H

#include <Arduino.h>
#include <ArduinoWebsockets.h>
#include <ArduinoJson.h>
#include <functional>
#include <unordered_map>


class WebSocketService: public websockets::WebsocketsClient
{
	public:
	typedef std::function<void(const String&)> MessageCallback;
	typedef std::function<bool(const char*)> IdentityCallback;

	class EventHandler {
		public:
		EventHandler(
			const char *eventKey,
			MessageCallback callback,
			const char *channel = NULL,
			IdentityCallback identityCallback = NULL
		);
		~EventHandler() = default;
		EventHandler(const EventHandler &other);
		EventHandler &operator=(const EventHandler &other);

		bool identify(const char *identity) const;
		void execute(const String &message);
		bool isEnabled() const;
		void setCallback(MessageCallback callback);

		bool filter(const char *channel) const;

		private:
		const char *eventKey;
		MessageCallback callback;
		IdentityCallback identityCallback;
		const char *channel;
		bool registered;
	};
	typedef std::unordered_map<std::string, EventHandler>::iterator EventHandlerIterator;

	public:
	WebSocketService(
		const String &url,
		void (*log)(const char *) = defaultLog
	);
	virtual ~WebSocketService();

	bool poll();

	void registerEventHandler(
		const char* eventName,
		MessageCallback callback
	) {
		eventHandlerMap.emplace(
			std::string(eventName),
			EventHandler(
				eventName,
				callback
			)
		);
	}

	void registerEventHandler(
		const char* eventName,
		MessageCallback callback,
		const char *channel
	) {
		eventHandlerMap.emplace(
			std::string(eventName),
			EventHandler(eventName, callback, channel)
		);
	}

	void registerEventHandler(
		const char* eventName,
		MessageCallback callback,
		const char *channel,
		IdentityCallback identityCallback
	) {
		eventHandlerMap.emplace(
			std::string(eventName),
			EventHandler(
				eventName,
				callback,
				channel,
				identityCallback
			)
		);
	}

	protected:
	void useHandleEvent(
		const char *eventKey,
		const String &message,
		const char *channel = NULL,
		const char *identity = NULL
	);	
	static void defaultLog(const char *message);

	public:

	struct MessageTraits {
		virtual const String type() = 0;
		virtual const String payload() = 0;
		virtual const String handlerHint() = 0;
		virtual const String senderIdentity() = 0;
	};

	class Message: public MessageTraits {
		public:
		Message(websockets::WebsocketsMessage);
		Message(const char *);

		const String type() override;
		const String payload() override;
		const String handlerHint() override;
		const String senderIdentity() override;
		const String getItem(const char *);
		// const String reducer() override;

		protected:
		StaticJsonDocument<256> json;
	};

	private:
		const String url;
		bool connected;
		template<class Message = WebSocketService::Message>
		void setupEventDriver();
		void handleConnection();
		void (*log)(const char *);

	private:
        std::unordered_map<std::string, EventHandler> eventHandlerMap;
};

#endif