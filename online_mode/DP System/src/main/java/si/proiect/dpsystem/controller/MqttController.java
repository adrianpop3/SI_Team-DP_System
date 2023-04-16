package si.proiect.dpsystem.controller;

import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.integration.support.MessageBuilder;
import org.springframework.messaging.Message;
import org.springframework.messaging.MessageChannel;
import org.springframework.scheduling.annotation.EnableScheduling;
import org.springframework.scheduling.annotation.Scheduled;
import org.springframework.stereotype.Controller;

@Controller
@EnableScheduling
public class MqttController {

    @Autowired
    private MessageChannel mqttOutputChannel;

    @Scheduled(fixedDelay = 1000) // send message every second
    public void sendMessage() {
        String payload = "Spring boot is sending a message!";
        Message<String> message = MessageBuilder.withPayload(payload).build();
        mqttOutputChannel.send(message);
    }

}
