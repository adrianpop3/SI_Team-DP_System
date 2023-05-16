package dp.system.back;

import org.springframework.context.annotation.ComponentScan;
import org.springframework.context.annotation.Configuration;

@Configuration
@ComponentScan(basePackages = "dp.system.back.controllers")
public class MqttBeans {

//    @Bean
//    public MqttPahoClientFactory mqttClientFactory() {
//
//        DefaultMqttPahoClientFactory factory = new DefaultMqttPahoClientFactory();
//        MqttConnectOptions options = new MqttConnectOptions();
//
//        options.setServerURIs(new String[]{"tcp://192.168.0.115:1883"});
//        options.setCleanSession(false);
//        factory.setConnectionOptions(options);
//
//        return factory;
//    }
//
//    @Bean
//    public MessageChannel mqttInputChannel() {
//        return new DirectChannel();
//    }
//
//    @Bean
//    public MessageProducer inbound() {
//        MqttPahoMessageDrivenChannelAdapter adapter =
//                new MqttPahoMessageDrivenChannelAdapter("springapplicationReceiver", mqttClientFactory(), "dpsystem");
//        adapter.setCompletionTimeout(5000);
//        adapter.setConverter(new DefaultPahoMessageConverter());
//        adapter.setQos(2);
//        adapter.setOutputChannel(mqttInputChannel());
//        return adapter;
//    }
//
//
//    // Actual processing of the message arrived on the mqttInputChannel
//    @Bean
//    @ServiceActivator(inputChannel = "mqttInputChannel")
//    public MessageHandler handler() {
//        return message -> System.out.println(message.getPayload());
//    }
//
//    @Bean
//    public MessageChannel mqttOutputChannel() {
//        return new DirectChannel();
//    }
//
//    @Bean
//    @ServiceActivator(inputChannel = "mqttOutputChannel")
//    public MessageHandler outbound() {
//        MqttPahoMessageHandler messageHandler = new MqttPahoMessageHandler("springapplicationSender", mqttClientFactory());
//        messageHandler.setAsync(true);
//        messageHandler.setDefaultTopic("dpsystem");
//        messageHandler.setDefaultRetained(false);
//        return messageHandler;
//    }
}
